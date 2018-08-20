#ifndef KVNSFER_HTTP_SERVER_HTTP_SERVER_RESPONSE_INFO_H
#define KVNSFER_HTTP_SERVER_HTTP_SERVER_RESPONSE_INFO_H

#include "http_status_code.h"
#include <string>
#include <vector>
#include <utility>

class HttpServerResponseInfo {
public:
  // Creates a 200 OK HttpServerResponseInfo.
  HttpServerResponseInfo();
  explicit HttpServerResponseInfo(HttpStatusCode status_code);
  HttpServerResponseInfo(const HttpServerResponseInfo& other);
  ~HttpServerResponseInfo();

  static HttpServerResponseInfo CreateFor404();
  static HttpServerResponseInfo CreateFor500(const std::string& body);

  void AddHeader(const std::string& name, const std::string& value);

  // This also adds an appropriate Content-Length header.
  void SetBody(const std::string& body, const std::string& content_type);
  // Sets content-length and content-type. Body should be sent separately.
  void SetContentHeaders(size_t content_length,
    const std::string& content_type);

  std::string Serialize() const;

  HttpStatusCode status_code() const;
  const std::string& body() const;

private:
  using Headers = std::vector<std::pair<std::string, std::string>>;

  HttpStatusCode status_code_;
  Headers headers_;
  std::string body_;
};

#endif // KVNSFER_HTTP_SERVER_HTTP_SERVER_RESPONSE_INFO_H
