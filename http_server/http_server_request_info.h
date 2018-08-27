#ifndef KVNSFER_HTTP_SERVER_HTTP_SERVER_REQUEST_INFO_H
#define KVNSFER_HTTP_SERVER_HTTP_SERVER_REQUEST_INFO_H

#include <string>
#include <map>

class HttpServerRequestInfo {
public:
  HttpServerRequestInfo();
  HttpServerRequestInfo(const HttpServerRequestInfo& other);
  ~HttpServerRequestInfo();

  // Returns header value for given header name. |header_name| should be
  // lower case.
  std::string GetHeaderValue(const std::string& header_name) const;

  // Checks for item in comma-separated header value for given header name.
  // Both |header_name| and |header_value| should be lower case.
  bool HasHeaderValue(
    const std::string& header_name,
    const std::string& header_value) const;

  // Request peer address.
  //wxIPV4address peer;

  // Request method.
  std::string method;

  // Request line.
  std::string path;

  // Request data.
  std::string data;

  // A map of the names -> values for HTTP headers. These should always
  // contain lower case field names.
  typedef std::map<std::string, std::string> HeadersMap;
  mutable HeadersMap headers;
};

#endif // KVNSFER_HTTP_SERVER_HTTP_SERVER_REQUEST_INFO_H
