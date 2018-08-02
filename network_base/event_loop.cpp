#include "event_loop.h"

EventLoop::Internal::Internal()
  :read_event(nullptr)
  , write_event(nullptr) {}

EventLoop::EventLoop(wxIPaddress& addr, Delegate* delegate)
  :wxThread(wxTHREAD_JOINABLE)
  , delegate_(delegate) {
  event_base_ = event_base_new();
  wxASSERT(event_base_);
  std::unique_ptr<wxSocketServer> socket_server =
    std::make_unique<wxSocketServer>(addr);
  socket_server->SetFlags(wxSOCKET_REUSEADDR | wxSOCKET_NOWAIT);
  Internal& item = id_to_sockets_[socket_server->GetSocket()];
  item.read_event = event_new(event_base_, socket_server->GetSocket(),
    EV_READ | EV_PERSIST, DoAccept, (void*)this);
  wxASSERT(item.read_event);
  event_add(item.read_event, nullptr);
  item.socket = std::move(socket_server);
}

EventLoop::~EventLoop() {
  event_base_free(event_base_);
}

void EventLoop::SetDelegate(Delegate* delegate) {
  delegate_ = delegate;
}

EventLoop::Delegate* EventLoop::GetDelegate() {
  return delegate_;
}

wxThread::ExitCode EventLoop::Entry() {
  event_base_dispatch(event_base_);
  return nullptr;
}

// static
void EventLoop::DoAccept(evutil_socket_t listener, short event, void *arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->DoAccept(listener, event);
}

void EventLoop::DoAccept(evutil_socket_t listener, short event) {
  auto& itsock = id_to_sockets_.find(listener);
  if (itsock != id_to_sockets_.end()) {
    wxSocketServer* socket_server = (wxSocketServer*)itsock->second.socket.get();
    std::unique_ptr<wxSocketBase> socket_peer = std::make_unique<wxSocketBase>();
    if (socket_server->AcceptWith(*socket_peer)) {
      if (delegate_)
        delegate_->OnAccept(socket_peer->GetSocket());
      socket_peer->SetFlags(wxSOCKET_NOWAIT);
      Internal& item = id_to_sockets_[socket_peer->GetSocket()];
      item.read_event = event_new(event_base_, socket_peer->GetSocket(), EV_READ | EV_PERSIST, DoRead, this);
      event_add(item.read_event, nullptr);
      item.socket = std::move(socket_peer);
    }
  }
}

// static
void EventLoop::DoRead(evutil_socket_t fd, short events, void *arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->DoRead(fd, events);
}

void EventLoop::DoRead(evutil_socket_t fd, short event) {
  std::map<evutil_socket_t, Internal>::iterator itsock
    = id_to_sockets_.find(fd);
  if (itsock != id_to_sockets_.end()) {
    wxSocketInputStream input(*itsock->second.socket);
    input.Peek();
    if (input.Eof()) {
      if (delegate_)
        delegate_->OnClose(fd);
      if (itsock->second.read_event)
        event_free(itsock->second.read_event);
      if (itsock->second.write_event)
        event_free(itsock->second.write_event);
      id_to_sockets_.erase(itsock);
      return;
    }
    if (delegate_)
      delegate_->OnRead(fd, input);
  }
}

// static
void EventLoop::DoWrite(evutil_socket_t fd, short events, void *arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->DoWrite(fd, events);
}

void EventLoop::DoWrite(evutil_socket_t fd, short events) {
  std::map<evutil_socket_t, Internal>::iterator itsock
    = id_to_sockets_.find(fd);
  if (itsock != id_to_sockets_.end()) {
    wxSocketOutputStream output(*itsock->second.socket);
    if (delegate_)
      delegate_->OnWrite(fd, output);
    if (output.LastWrite() > 0)
      event_add(itsock->second.write_event, nullptr);
  }
}

void EventLoop::DoConnect(evutil_socket_t fd, short event) {
  std::map<evutil_socket_t, Internal>::iterator itsock
    = id_to_sockets_.find(fd);
  if (itsock != id_to_sockets_.end()) {
    if (delegate_)
      delegate_->OnConnect(fd);
  }
}

// static
void EventLoop::DoConnect(evutil_socket_t fd, short events, void* arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->DoConnect(fd, events);
}

bool EventLoop::SetNeedWrite(wxUint32 id) {
  std::map<evutil_socket_t, Internal>::iterator itsock
    = id_to_sockets_.find(id);
  if (itsock != id_to_sockets_.end()) {
    if(!itsock->second.write_event)
      itsock->second.write_event = event_new(
        event_base_, itsock->second.socket->GetSocket(), 
        EV_WRITE, DoWrite, this);
    event_add(itsock->second.write_event, nullptr);
    return true;
  }
  return false;
}

wxUint32 EventLoop::Connect(wxIPaddress& peer) {
  std::unique_ptr<wxSocketClient> socket_client = std::make_unique<wxSocketClient>();
  socket_client->SetFlags(wxSOCKET_NOWAIT);
  bool connected = socket_client->Connect(peer);
  Internal& item = id_to_sockets_[socket_client->GetSocket()];
  item.socket = std::move(socket_client);
  if (connected) {
    DoConnect(item.socket->GetSocket(), EV_WRITE, this);
  }
  else {
    item.write_event = event_new(event_base_, item.socket->GetSocket(), EV_WRITE, DoWrite, this);
    event_add(item.write_event, nullptr);
  }
  item.read_event = event_new(event_base_, item.socket->GetSocket(), EV_READ | EV_PERSIST, DoRead, this);
  event_add(item.read_event, nullptr);
}

void EventLoop::Close(wxUint32 id) {
  std::map<evutil_socket_t, Internal>::iterator itsock
    = id_to_sockets_.find(id);
  if (itsock != id_to_sockets_.end()) {
    if (itsock->second.read_event)
      event_free(itsock->second.read_event);
    if (itsock->second.write_event)
      event_free(itsock->second.write_event);
    id_to_sockets_.erase(id);
    if (delegate_)
      delegate_->OnClose(id);
  }
}