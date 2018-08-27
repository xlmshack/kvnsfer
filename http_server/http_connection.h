#ifndef KVNSFER_HTTP_SERVER_HTTP_CONNECTION_H_
#define KVNSFER_HTTP_SERVER_HTTP_CONNECTION_H_

#include <string>
#include <apr.h>

class HttpConnection {
public:
  HttpConnection(apr_uint32_t id);
  ~HttpConnection();

  apr_uint32_t GetId();
  std::string& GetReadBuf();
  std::string& GetWriteBuf();

private:
  const apr_uint32_t id_;
  std::string read_buf_;
  std::string write_buf_;

  HttpConnection(const HttpConnection&) = delete;
  HttpConnection& operator=(const HttpConnection&) = delete;
};
#endif // KVNSFER_HTTP_SERVER_HTTP_CONNECTION_H_
