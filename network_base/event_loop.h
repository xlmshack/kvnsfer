#ifndef KVNSFER_NETWORK_BASE_EVENT_LOOP_H_
#define KVNSFER_NETWORK_BASE_EVENT_LOOP_H_

#include <wx/defs.h>
#include <wx/thread.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
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
    virtual void OnAccept(wxUint32 id) = 0;
    virtual void OnConnect(wxUint32 id) = 0;
    virtual void OnRead(wxUint32 id, const char* buf, wxUint32 size) = 0;
    virtual void OnWrite(wxUint32 id) = 0;
    virtual void OnClose(wxUint32 id) = 0;
  };

  EventLoop(const std::string& addr, uint16_t port, Delegate* delegate);
  ~EventLoop() wxOVERRIDE;

  void SetDelegate(Delegate* delegate);
  Delegate* GetDelegate();
  bool SetNeedWrite(wxUint32 id);
  wxUint32 Connect(const std::string& addr, uint16_t port);
  void Write(wxUint32 id);
  void Close(wxUint32 id);

protected:
  // wxThread
  virtual ExitCode Entry() wxOVERRIDE;

  static void do_listener_cb(struct evconnlistener* listener, 
    evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg);
  void do_listener_cb(struct evconnlistener* listener,
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
  std::string address_;
  uint16_t port_;
  std::map<evutil_socket_t, Internal> id_to_sockets_;

  wxDECLARE_NO_COPY_CLASS(EventLoop);
};

#endif // KVNSFER_NETWORK_BASE_EVENT_LOOP_H_
