#include "http_connection.h"

HttpConnection::HttpConnection(apr_uint32_t id)
  :id_(id) {}

HttpConnection::~HttpConnection() {}

apr_uint32_t HttpConnection::GetId() {
  return id_;
}

std::string& HttpConnection::GetReadBuf() {
  return read_buf_;
}

std::string& HttpConnection::GetWriteBuf() {
  return write_buf_;
}