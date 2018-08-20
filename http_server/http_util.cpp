#include "http_util.h"

namespace {
template <typename ConstIterator>
void TrimLWSImplementation(ConstIterator* begin, ConstIterator* end) {
  // leading whitespace
  while (*begin < *end && HttpUtil::IsLWS((*begin)[0]))
    ++(*begin);

  // trailing whitespace
  while (*begin < *end && HttpUtil::IsLWS((*end)[-1]))
    --(*end);
}
}  // namespace

bool HttpUtil::IsLWS(char c) {
  const std::string kWhiteSpaceCharacters(HTTP_LWS);
  return kWhiteSpaceCharacters.find(c) != std::string::npos;
}

// static
std::string HttpUtil::TrimLWS(const std::string& string) {
  const char* begin = string.data();
  const char* end = string.data() + string.size();
  TrimLWSImplementation(&begin, &end);
  return std::string(begin, end - begin);
}

bool HttpUtil::IsTokenChar(char c) {
  return !(c >= 0x7F || c <= 0x20 || c == '(' || c == ')' || c == '<' ||
    c == '>' || c == '@' || c == ',' || c == ';' || c == ':' ||
    c == '\\' || c == '"' || c == '/' || c == '[' || c == ']' ||
    c == '?' || c == '=' || c == '{' || c == '}');
}

// See RFC 7230 Sec 3.2.6 for the definition of |token|.
bool HttpUtil::IsToken(const std::string& string) {
  if (string.empty())
    return false;
  for (char c : string) {
    if (!IsTokenChar(c))
      return false;
  }
  return true;
}

// static
bool HttpUtil::IsValidHeaderName(const std::string& name) {
  // Check whether the header name is RFC 2616-compliant.
  return HttpUtil::IsToken(name);
}

// static
bool HttpUtil::IsValidHeaderValue(const std::string& value) {
  // Just a sanity check: disallow NUL, CR and LF.
  for (char c : value) {
    if (c == '\0' || c == '\r' || c == '\n')
      return false;
  }
  return true;
}