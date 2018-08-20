#ifndef KVNSFER_HTTP_SERVER_HTTP_UTIL_H_
#define KVNSFER_HTTP_SERVER_HTTP_UTIL_H_

#include <string>

#define HTTP_LWS " \t"

class HttpUtil {
public:
  static bool IsLWS(char c);
  static std::string TrimLWS(const std::string& string);
  // Whether the character is a valid |tchar| as defined in RFC 7230 Sec 3.2.6.
  static bool IsTokenChar(char c);
  // Whether the string is a valid |token| as defined in RFC 7230 Sec 3.2.6.
  static bool IsToken(const std::string& str);
  // Returns true if |name| is a valid HTTP header name.
  static bool IsValidHeaderName(const std::string& name);
  // Returns false if |value| contains NUL or CRLF. This method does not perform
  // a fully RFC-2616-compliant header value validation.
  static bool IsValidHeaderValue(const std::string& value);
};


#endif // KVNSFER_HTTP_SERVER_HTTP_UTIL_H_
