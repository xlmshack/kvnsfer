#include "http_server.h"

HttpServer::HttpServer(wxIPaddress& addr)
  :EventLoop(addr, this) {

}

HttpServer::~HttpServer() {

}

void HttpServer::OnConnect(wxUint32 id) {

}

void HttpServer::OnRead(wxUint32 id, wxSocketInputStream& stream) {

}

void HttpServer::OnWrite(wxUint32 id, wxSocketOutputStream& stream) {

}

void HttpServer::OnClose(wxUint32 id) {

}