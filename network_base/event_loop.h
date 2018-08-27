#ifndef KVNSFER_NETWORK_BASE_EVENT_LOOP_H_
#define KVNSFER_NETWORK_BASE_EVENT_LOOP_H_

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <event2/util.h>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "base/thread.h"
#include <apr.h>

class EventLoop : public base::Thread {
private:
  struct Internal {
    Internal();
    struct bufferevent *bev;
  };
public:
  class Delegate {
  public:
    virtual ~Delegate() {}
    virtual void OnAccept(apr_uint32_t id, const std::string& addr, apr_uint16_t port) = 0;
    virtual void OnConnect(apr_uint32_t id) = 0;
    virtual void OnRead(apr_uint32_t id, const char* buffer, apr_uint32_t size) = 0;
    virtual void OnWrite(apr_uint32_t id) = 0;
    virtual void OnClose(apr_uint32_t id) = 0;
  };

  EventLoop(Delegate* delegate);
  ~EventLoop() override;

  void SetDelegate(Delegate* delegate);
  Delegate* GetDelegate();
  bool Listen(const std::string& addr, apr_uint16_t* port, int backlog);
  apr_uint32_t Connect(const std::string& addr, apr_uint16_t port);
  void Write(apr_uint32_t id, const char* buffer, apr_uint32_t size);
  void Close(apr_uint32_t id);
  void Exit();

protected:
  // wxThread
  virtual void Entry() override;

  static void do_accept_cb(struct evconnlistener* listener, 
    evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg);
  void do_accept_cb(struct evconnlistener* listener,
    evutil_socket_t fd, struct sockaddr* addr, int socklen);
  static void do_read_cb(struct bufferevent *bev, void *arg);
  void do_read_cb(struct bufferevent *bev);
  static void do_write_cb(struct bufferevent *bev, void *arg);
  void do_write_cb(struct bufferevent *bev);
  static void do_event_cb(struct bufferevent *bev, short what, void *arg);
  void do_event_cb(struct bufferevent *bev, short what);
  
private:
  Delegate* delegate_;
  struct event_base* event_base_;
  std::map<evutil_socket_t, Internal> id_to_sockets_;

  EventLoop(const EventLoop&) = delete;
  EventLoop& operator=(const EventLoop&) = delete;
};

#endif // KVNSFER_NETWORK_BASE_EVENT_LOOP_H_
