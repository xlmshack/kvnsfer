#include "event_loop.h"

EventLoop::EventLoop(wxIPaddress& addr, Delegate* delegate)
  :wxThread(wxTHREAD_JOINABLE)
  , socket_server_(addr)
  , delegate_(delegate) {
  event_base_ = event_base_new();
  wxASSERT(event_base_);
  evutil_make_socket_nonblocking(socket_server_.GetSocket());
  socket_server_.SetFlags(wxSOCKET_REUSEADDR);
}

EventLoop::~EventLoop() {

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
  std::unique_ptr<wxSocketBase> socket_client = std::make_unique<wxSocketBase>();
  if (socket_server_.AcceptWith(*socket_client)) {
    if (delegate_)
      delegate_->OnConnect(listener);
    evutil_make_socket_nonblocking(socket_client->GetSocket());
    struct event *read_event = event_new(event_base_, socket_client->GetSocket(), EV_READ | EV_PERSIST, DoRead, this);
    event_add(read_event, nullptr);
    struct event *write_event = event_new(event_base_, socket_client->GetSocket(), EV_WRITE | EV_PERSIST, DoWrite, this);
    event_add(write_event, nullptr);
    id_to_sockets_[socket_client->GetSocket()] = std::move(socket_client);
  }
}

// static
void EventLoop::DoRead(evutil_socket_t fd, short events, void *arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->DoRead(fd, events);
}

void EventLoop::DoRead(evutil_socket_t fd, short event) {
  std::map<evutil_socket_t, std::unique_ptr<wxSocketBase> >::iterator itsock
    = id_to_sockets_.find(fd);
  if (itsock != id_to_sockets_.end()) {
    wxSocketInputStream input(*itsock->second);
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
  std::map<evutil_socket_t, std::unique_ptr<wxSocketBase> >::iterator itsock
    = id_to_sockets_.find(fd);
  if (itsock != id_to_sockets_.end()) {
    wxSocketOutputStream output(*itsock->second);
    if (delegate_)
      delegate_->OnWrite(fd, output);
  }
}

