#ifndef KVNSFER_NETWORK_BASE_EVENT_LOOP_H_
#define KVNSFER_NETWORK_BASE_EVENT_LOOP_H_

#include <wx/defs.h>
#include <wx/thread.h>
#include <wx/buffer.h>
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

class EventLoop : public wxThread {
private:
  struct Internal {
    Internal();
    struct bufferevent *bev;
  };
public:
  class Delegate {
  public:
    virtual ~Delegate() {}
    virtual void OnAccept(wxUint32 id, const std::string& addr, wxUint16 port) = 0;
    virtual void OnConnect(wxUint32 id) = 0;
    virtual void OnRead(wxUint32 id, const char* buffer, wxUint32 size) = 0;
    virtual void OnWrite(wxUint32 id) = 0;
    virtual void OnClose(wxUint32 id) = 0;
  };

  EventLoop(Delegate* delegate);
  ~EventLoop() wxOVERRIDE;

  void SetDelegate(Delegate* delegate);
  Delegate* GetDelegate();
  bool Listen(const std::string& addr, uint16_t* port, int backlog);
  wxUint32 Connect(const std::string& addr, uint16_t port);
  void Write(wxUint32 id, const char* buffer, wxUint32 size);
  void Close(wxUint32 id);
  void Exit();

protected:
  // wxThread
  virtual ExitCode Entry() wxOVERRIDE;

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
  wxDECLARE_NO_COPY_CLASS(EventLoop);
};

#endif // KVNSFER_NETWORK_BASE_EVENT_LOOP_H_
