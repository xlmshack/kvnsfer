#include "event_loop.h"
#include "winsock_init.h"

EventLoop::Internal::Internal()
  :bev(nullptr) {}

EventLoop::EventLoop(Delegate* delegate)
  : wxThread(wxTHREAD_JOINABLE)
  , delegate_(delegate) {
  EnsureWinsockInit();
#ifdef _WIN32
  evthread_use_windows_threads();
#else
  evthread_use_pthreads();
#endif
  event_base_ = event_base_new();
}

EventLoop::~EventLoop() {
  if(event_base_)
    event_base_free(event_base_);
}

void EventLoop::SetDelegate(Delegate* delegate) {
  delegate_ = delegate;
}

EventLoop::Delegate* EventLoop::GetDelegate() {
  return delegate_;
}

bool EventLoop::Listen(const std::string& addr, uint16_t* port, int backlog) {
  wxASSERT(!addr.empty());
  wxASSERT(port);
  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  evutil_inet_pton(AF_INET, addr.c_str(), &sin.sin_addr.s_addr);
  sin.sin_port = htons(*port);
  // There is a memory leak at |listener| without freeing.
  struct evconnlistener* listener = evconnlistener_new_bind(event_base_, do_accept_cb, this,
    LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, backlog, (struct sockaddr*)&sin, sizeof(sin));
  return listener != nullptr;
}

wxUint32 EventLoop::Connect(const std::string& addr, uint16_t port) {
  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  evutil_inet_pton(AF_INET, addr.c_str(), &sin.sin_addr.s_addr);
  sin.sin_port = htons(port);
  struct bufferevent* bev = bufferevent_socket_new(event_base_, -1, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, do_read_cb, do_write_cb, do_event_cb, this);
  bufferevent_socket_connect(bev, (const sockaddr*)&sin, sizeof(sin));
  evutil_socket_t fd = bufferevent_getfd(bev);
  bufferevent_enable(bev, EV_READ | EV_WRITE);
  id_to_sockets_[fd].bev = bev;
  return fd;
}

void EventLoop::Write(wxUint32 id, const char* buffer, wxUint32 size) {
  auto itbev = id_to_sockets_.find(id);
  if (itbev != id_to_sockets_.end()) {
    evbuffer* output = bufferevent_get_output(itbev->second.bev);
    evbuffer_add(output, buffer, size);
    bufferevent_enable(itbev->second.bev, EV_WRITE | EV_READ);
  }
}

void EventLoop::Close(wxUint32 id) {
  auto itbev = id_to_sockets_.find(id);
  if (itbev != id_to_sockets_.end()) {
    bufferevent_free(itbev->second.bev);
  }
}

wxThread::ExitCode EventLoop::Entry() {
  return (ExitCode)event_base_dispatch(event_base_);
}

// static
void EventLoop::do_accept_cb(struct evconnlistener* listener,
  evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg) {
  wxASSERT(arg);
  EventLoop* cur = (EventLoop*)arg;
  cur->do_accept_cb(listener, fd, addr, socklen);
}

void EventLoop::do_accept_cb(struct evconnlistener* listener,
  evutil_socket_t fd, struct sockaddr* addr, int socklen) {
  if (delegate_) {
    char addrbuf[16];
    evutil_inet_ntop(AF_INET, addr, addrbuf, sizeof(addrbuf));
    delegate_->OnAccept(fd, std::string(addrbuf), ntohs(((sockaddr_in*)addr)->sin_port));
  }
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
  wxMemoryBuffer buffer;
  while (evbuffer_get_length(input) > 0) {
    char buf[1024];
    int nread = evbuffer_remove(input, buf, sizeof(buf));
    if (nread > 0)
      buffer.AppendData(buf, nread);
  }
  if (buffer.GetDataLen() > 0 && delegate_)
    delegate_->OnRead(fd, (const char*)buffer.GetData(), buffer.GetDataLen());
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
  evutil_socket_t fd = bufferevent_getfd(bev);
  auto itbev = id_to_sockets_.find(fd);
  if (itbev != id_to_sockets_.end()) {
    if (what & BEV_EVENT_CONNECTED) {
      if (delegate_)
        delegate_->OnConnect(fd);
    }
    else if (what & BEV_EVENT_EOF) {
      if (delegate_)
        delegate_->OnClose(fd);
    }
  }
}
