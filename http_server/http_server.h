#ifndef KVNSFER_HTTP_SERVER_HTTP_SERVER_H_
#define KVNSFER_HTTP_SERVER_HTTP_SERVER_H_

#include <map>
#include "event_loop.h"
#include "wx/defs.h"

class HttpServerRequestInfo;
class HttpConnection;

class HttpServer : public EventLoop::Delegate {
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
  HttpServer(const std::string& vir_host);
  ~HttpServer();

  void StartServer(const std::string& addr, wxUint16* port);
  void StopServer();

  // EventLoop::Delegate
  void OnAccept(wxUint32 id, const std::string& addr, wxUint16 port) wxOVERRIDE;
  void OnRead(wxUint32 id, const char* buffer, wxUint32 size) wxOVERRIDE;
  void OnWrite(wxUint32 id) wxOVERRIDE;
  void OnClose(wxUint32 id) wxOVERRIDE;

protected:
  bool ParseHeaders(const char* data, size_t data_len,
    HttpServerRequestInfo* info, size_t* ppos);

private:
  EventLoop event_loop_;
  std::map<wxUint32, std::unique_ptr<HttpConnection> > id_to_connection_;
  wxUint32 last_id_;
};

#endif // KVNSFER_HTTP_SERVER_HTTP_SERVER_H_