#ifndef KVNSFER_HTTP_SERVER_HTTP_SERVER_H_
#define KVNSFER_HTTP_SERVER_HTTP_SERVER_H_

#include "event_loop.h"
#include "wx/defs.h"

class HttpServerRequestInfo;

class HttpServer : public EventLoop
                 , public EventLoop::Delegate {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}
    virtual void OnConnect(int connection_id) = 0;
    virtual void OnHttpRequest(int connection_id,
      const HttpServerRequestInfo& info) = 0;
    virtual void OnClose(int connection_id) = 0;
  };
  // ctor & dtor
  HttpServer(wxIPaddress& addr);
  ~HttpServer();

  // EventLoop::Delegate
  void OnConnect(wxUint32 id) wxOVERRIDE;
  void OnRead(wxUint32 id, wxSocketInputStream& stream) wxOVERRIDE;
  void OnWrite(wxUint32 id, wxSocketOutputStream& stream) wxOVERRIDE;
  void OnClose(wxUint32 id) wxOVERRIDE;
};

#endif // KVNSFER_HTTP_SERVER_HTTP_SERVER_H_