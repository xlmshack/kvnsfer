#include "event_loop.h"

EventLoop::EventLoop(wxIPaddress& addr, Delegate* delegate)
  :wxThread(wxTHREAD_JOINABLE)
  , socket_server_(addr)
  , delegate_(delegate) {
  event_base_ = event_base_new();
  wxASSERT(event_base_);
  socket_server_.SetFlags(wxSOCKET_REUSEADDR);
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
  struct event* listener_event = 
    event_new(event_base_, socket_server_.GetSocket(), 
      EV_READ | EV_PERSIST, DoAccept, (void*)this);
  event_add(listener_event, nullptr);
  int ret = event_base_dispatch(event_base_);
  ret = WSAGetLastError();
  return nullptr;
}

// static
void EventLoop::DoAccept(evutil_socket_t listener, short event, void *arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->DoAccept(listener, event);
}

void EventLoop::DoAccept(evutil_socket_t listener, short event) {
  std::unique_ptr<wxSocketBase> socket_client = std::make_unique<wxSocketBase>();
  if (socket_server_.AcceptWith(*socket_client)) {
    if (delegate_)
      delegate_->OnConnect(socket_client->GetSocket());
    socket_client->SetFlags(wxSOCKET_NOWAIT);
    Internal& item = id_to_sockets_[socket_client->GetSocket()];
    item.read_event = event_new(event_base_, socket_client->GetSocket(), EV_READ | EV_PERSIST, DoRead, this);
    event_add(item.read_event, nullptr);
    item.write_event = event_new(event_base_, socket_client->GetSocket(), EV_WRITE, DoWrite, this);
    event_add(item.write_event, nullptr);
    item.socket_client = std::move(socket_client);
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
    wxSocketInputStream input(*itsock->second.socket_client);
    input.Peek();
    if (input.Eof()) {
      if (delegate_)
        delegate_->OnClose(fd);
      event_free(itsock->second.read_event);
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
    wxSocketOutputStream output(*itsock->second.socket_client);
    if (delegate_)
      delegate_->OnWrite(fd, output);
    if (output.LastWrite() > 0)
      event_add(itsock->second.write_event, nullptr);
  }
}

bool EventLoop::Write(wxUint32 id, const void *buffer, size_t size) {
  std::map<evutil_socket_t, Internal>::iterator itsock
    = id_to_sockets_.find(id);
  if (itsock != id_to_sockets_.end()) {
    itsock->second.socket_client->Write(buffer, size);
    event_add(itsock->second.write_event, nullptr);
    return true;
  }
  return false;
}