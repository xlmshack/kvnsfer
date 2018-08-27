#ifndef KVNSFER_HTTP_SERVER_STRING_UTIL_H_
#define KVNSFER_HTTP_SERVER_STRING_UTIL_H_

#include <string>

namespace base {

char ToLowerASCII(char c);
std::string ToLowerASCII(std::string str);
int CompareCaseInsensitiveASCII(std::string a, std::string b);
bool EqualsCaseInsensitiveASCII(std::string a, std::string b);

enum TrimPositions {
  TRIM_NONE = 0,
  TRIM_LEADING = 1 << 0,
  TRIM_TRAILING = 1 << 1,
  TRIM_ALL = TRIM_LEADING | TRIM_TRAILING,
};

std::string TrimString(std::string input,
  const std::string& trim_chars,
  TrimPositions positions);

std::string TrimWhitespace(std::string input,
  TrimPositions positions);

std::string StringPrintf(const char* format, ...);

void StringAppendF(std::string* dst, const char* format, ...);

bool StringToSizeT(const std::string& input, size_t* output);

} // namespace base

#endif // KVNSFER_HTTP_SERVER_STRING_UTIL_H_
