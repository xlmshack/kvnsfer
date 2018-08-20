#include "string_util.h"
#include <algorithm>
#include <wx/string.h>

namespace base {

int CompareCaseInsensitiveASCII(std::string a, std::string b) {
  // Find the first characters that aren't equal and compare them.  If the end
  // of one of the strings is found before a nonequal character, the lengths
  // of the strings are compared.
  size_t i = 0;
  while (i < a.length() && i < b.length()) {
    typename std::string::value_type lower_a = ToLowerASCII(a[i]);
    typename std::string::value_type lower_b = ToLowerASCII(b[i]);
    if (lower_a < lower_b)
      return -1;
    if (lower_a > lower_b)
      return 1;
    i++;
  }

  // End of one string hit before finding a different character. Expect the
  // common case to be "strings equal" at this point so check that first.
  if (a.length() == b.length())
    return 0;

  if (a.length() < b.length())
    return -1;
  return 1;
}

bool EqualsCaseInsensitiveASCII(std::string a, std::string b) {
  if (a.length() != b.length())
    return false;
  return CompareCaseInsensitiveASCII(a, b) == 0;
}

char ToLowerASCII(char c) {
  return tolower(c);
}

std::string ToLowerASCII(std::string str) {
  std::string ret;
  std::transform(str.begin(), str.end(), ret.begin(), tolower);
  return ret;
}

std::string TrimString(std::string input,
  const std::string& trim_chars,
  TrimPositions positions) {
  if (positions == TrimPositions::TRIM_NONE)
    return input;
  if (positions | TrimPositions::TRIM_LEADING) {
    std::string::size_type left_pos = input.find_first_not_of(trim_chars);
    if (left_pos != std::string::npos) {
      input = input.substr(left_pos, input.size() - left_pos);
    }
  }
  if (positions | TrimPositions::TRIM_TRAILING) {
    std::string::size_type right_pos = input.find_last_not_of(trim_chars);
    if (right_pos != std::string::npos) {
      input = input.substr(0, right_pos + 1);
    }
  }
  return input;
}

std::string TrimWhitespace(std::string input,
  TrimPositions positions) {
  return TrimString(input, " ", TrimPositions::TRIM_ALL);
}

void StringAppendF(std::string* dst, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  wxString str;
  str.PrintfV(format, ap);
  dst->append(str.ToStdString());
  va_end(ap);
}

}