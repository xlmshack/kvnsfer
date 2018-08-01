#include "http_server.h"

HttpServer::HttpServer(wxIPaddress& addr)
  :event_loop_(addr, this) {

}

HttpServer::~HttpServer() {

}

void HttpServer::OnAccept(wxUint32 id) {

}

void HttpServer::OnRead(wxUint32 id, wxInputStream& stream) {

}

void HttpServer::OnWrite(wxUint32 id, wxOutputStream& stream) {

}

void HttpServer::OnClose(wxUint32 id) {

}