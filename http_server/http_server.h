#ifndef KVNSFER_HTTP_SERVER_HTTP_SERVER_H_
#define KVNSFER_HTTP_SERVER_HTTP_SERVER_H_

#include <map>
#include "event_loop.h"

class HttpServerRequestInfo;
class HttpServerResponseInfo;
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
  HttpServer(const std::string& vir_host, Delegate* delegate);
  ~HttpServer();

  void StartServer(const std::string& addr, apr_uint16_t* port);
  void StopServer();

  // EventLoop::Delegate
  void OnAccept(apr_uint32_t id, const std::string& addr, apr_uint16_t port) override;
  void OnConnect(apr_uint32_t id) override;
  void OnRead(apr_uint32_t id, const char* buffer, apr_uint32_t size) override;
  void OnWrite(apr_uint32_t id) override;
  void OnClose(apr_uint32_t id) override;

  void SendRaw(int connection_id, const std::string& data);
  void SendResponse(int connection_id, const HttpServerResponseInfo& response);

protected:
  bool ParseHeaders(const char* data, size_t data_len,
    HttpServerRequestInfo* info, size_t* ppos);

private:
  EventLoop event_loop_;
  std::map<apr_uint32_t, std::unique_ptr<HttpConnection> > id_to_connection_;
  apr_uint32_t last_id_;
  Delegate* const delegate_;

  HttpServer(const HttpServer&) = delete;
  HttpServer& operator=(const HttpServer&) = delete;
};

#endif // KVNSFER_HTTP_SERVER_HTTP_SERVER_H_