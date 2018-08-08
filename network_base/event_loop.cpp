#include "event_loop.h"

EventLoop::Internal::Internal()
  :bev(nullptr) {}

EventLoop::EventLoop(const std::string& addr, uint16_t port, Delegate* delegate)
  : wxThread(wxTHREAD_JOINABLE)
  , delegate_(delegate)
  , event_base_(event_base_new())
  , address_(addr)
  , port_(port_) {}

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
  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr(address_.c_str());
  sin.sin_port = htons(port_);
  struct evconnlistener* listener = evconnlistener_new_bind(event_base_, do_listener_cb, this,
    LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));
  if (!listener)
    return nullptr;
  event_base_dispatch(event_base_);
  evconnlistener_free(listener);
  event_base_free(event_base_);
  return nullptr;
}

bool EventLoop::SetNeedWrite(wxUint32 id) {
  std::map<evutil_socket_t, Internal>::iterator itsock
    = id_to_sockets_.find(id);
  if (itsock != id_to_sockets_.end()) {
    if (!itsock->second.write_event)
      itsock->second.write_event = event_new(
        event_base_, itsock->second.fd,
        EV_WRITE | EV_PERSIST, DoWrite, this);
    event_add(itsock->second.write_event, nullptr);
    return true;
  }
  return false;
}

wxUint32 EventLoop::Connect(const std::string& addr, uint16_t port) {
  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr(addr.c_str());
  sin.sin_port = htons(port);
  evutil_socket_t fd = socket(AF_INET, SOCK_STREAM, 0);
  evutil_make_socket_nonblocking(fd);
  Internal& item = id_to_sockets_[fd];
  item.fd = fd;
  item.write_event = event_new(event_base_, fd, EV_WRITE | EV_PERSIST, DoWrite, this);
  item.read_event = event_new(event_base_, fd, EV_READ | EV_PERSIST, DoRead, this);
  event_add(item.read_event, nullptr);
  event_add(item.write_event, nullptr);
  connect(fd, (const sockaddr*)&sin, sizeof(sin));
  return fd;
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

// static
void EventLoop::do_listener_cb(struct evconnlistener* listener,
  evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->do_listener_cb(listener, fd, addr, socklen);
}

void EventLoop::do_listener_cb(struct evconnlistener* listener,
  evutil_socket_t fd, struct sockaddr* addr, int socklen) {
  if (delegate_)
    delegate_->OnAccept(fd);
  evutil_make_socket_nonblocking(fd);
  struct bufferevent *bev = bufferevent_socket_new(event_base_, fd, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, do_read_cb, do_write_cb, do_event_cb, this);
  bufferevent_enable(bev, EV_READ | EV_WRITE);
  id_to_sockets_[fd].bev = bev;
}

// static
void EventLoop::do_read_cb(struct bufferevent *bev, void *arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->do_read_cb(bev);
}

void EventLoop::do_read_cb(struct bufferevent *bev) {
  struct evbuffer *input = bufferevent_get_input(bev);
  evutil_socket_t fd = bufferevent_getfd(bev);
  char buf[1024];
  while (evbuffer_get_length(input) > 0) {
    int nread = evbuffer_remove(input, buf, sizeof(buf));
    if (delegate_)
      delegate_->OnRead(fd, buf, nread);
  }
}

// static
void EventLoop::do_write_cb(struct bufferevent *bev, void *arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->do_write_cb(bev);
}

void EventLoop::do_write_cb(struct bufferevent *bev) {
  evutil_socket_t fd = bufferevent_getfd(bev);
  if (delegate_)
    delegate_->OnWrite(fd);
}

// static
void EventLoop::do_event_cb(struct bufferevent *bev, short what, void *arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->do_event_cb(bev, what);
}

void EventLoop::do_event_cb(struct bufferevent *bev, short what) {

}

void EventLoop::Write(wxUint32 id) {
  auto itbev = id_to_sockets_.find(id);
  if (itbev != id_to_sockets_.end()) {

  }
}