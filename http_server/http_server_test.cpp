#include "http_server.h"
#include "http_server_request_info.h"
#include "http_server_response_info.h"
#include <iostream>
#include "base/string_util.h"
#include <wx/thread.h>
#include <wx/init.h>
#include "http_status_code.h"
#include "base/apr_init.h"

class TransferServer : public HttpServer::Delegate {
public:
  TransferServer()
    :server_("", this) {
    wxUint16 port = 1990;
    server_.StartServer("0.0.0.0", &port);
  }

  ~TransferServer() {
    server_.StopServer();
  }

  void OnConnect(int connection_id) override {
    std::cout << "TransferServer::OnConnect()" << std::endl;
    std::cout << "connection id:" << connection_id << std::endl;
  }

  void OnHttpRequest(int connection_id,
    const HttpServerRequestInfo& info) override {
    std::cout << "TransferServer::OnHttpRequest()" << std::endl;
    HttpServerResponseInfo response(HTTP_OK);
    std::string body = "hello world";
    std::string content_type = "text/plain";
    response.SetBody(body, content_type);
    server_.SendResponse(connection_id, response);
  }

  void OnClose(int connection_id) override {
    std::cout << "TransferServer::OnClose()" << std::endl;
  }

private:
  HttpServer server_;
};

int main(int argc, char* argv[]) {
  base::EnsureAprInit();
  wxInitializer init;
  wxMutex mutex;
  wxCondition cond(mutex);
  TransferServer transfer;
  cond.Wait();
  return 0;
}