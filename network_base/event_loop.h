#ifndef KVNSFER_NETWORK_BASE_EVENT_LOOP_H_
#define KVNSFER_NETWORK_BASE_EVENT_LOOP_H_

#include <wx/defs.h>
#include <wx/thread.h>
#include <wx/sckstrm.h>
#include <wx/socket.h>
#include <event2/event.h>
#include <map>
#include <memory>

class EventLoop : public wxThread {
public:
  class Delegate {
  public:
    virtual void OnConnect(wxUint32 id) = 0;
    virtual void OnRead(wxUint32 id, wxSocketInputStream& stream) = 0;
    virtual void OnWrite(wxUint32 id, wxSocketOutputStream& stream) = 0;
    virtual void OnClose(wxUint32 id) = 0;
  };
  EventLoop(wxIPaddress& addr, Delegate* delegate);
  ~EventLoop();

  void SetDelegate(Delegate* delegate);
  Delegate* GetDelegate();

protected:
  // wxThread
  virtual ExitCode Entry() wxOVERRIDE;

  void DoAccept(evutil_socket_t listener, short event);
  static void DoAccept(evutil_socket_t listener, short event, void *arg);
  void DoRead(evutil_socket_t listener, short event);
  static void DoRead(evutil_socket_t fd, short events, void *arg);
  void DoWrite(evutil_socket_t fd, short events);
  static void DoWrite(evutil_socket_t fd, short events, void *arg);

private:
  wxSocketServer socket_server_;
  Delegate* delegate_;
  struct event_base* event_base_;
  std::map<evutil_socket_t, std::unique_ptr<wxSocketBase> > id_to_sockets_;

  wxDECLARE_NO_COPY_CLASS(EventLoop);
};

#endif // KVNSFER_NETWORK_BASE_EVENT_LOOP_H_
