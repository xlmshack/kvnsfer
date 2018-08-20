#include "http_connection.h"

HttpConnection::HttpConnection(wxUint32 id)
  :id_(id) {}

HttpConnection::~HttpConnection() {}

wxUint32 HttpConnection::GetId() {
  return id_;
}

wxMemoryBuffer& HttpConnection::GetReadBuf() {
  return read_buf_;
}

wxMemoryBuffer& HttpConnection::GetWriteBuf() {
  return write_buf_;
}