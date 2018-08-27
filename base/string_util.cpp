#include "string_util.h"
#include <algorithm>
#include <iterator>
#include <apr_lib.h>
#include <apr_cstr.h>

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
  std::transform(str.begin(), str.end(), std::back_inserter(ret), tolower);
  return ret;
}

std::string TrimString(std::string input,
  const std::string& trim_chars,
  TrimPositions positions) {
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

namespace {

struct apr_vformatter_buff_t_ex : public apr_vformatter_buff_t {
  apr_vformatter_buff_t_ex()
    :buffer(1024, '\xcc') {
    curpos = &buffer[0];
    endpos = &buffer[1024];
  }
  void reset() {
    curpos = &buffer[0];
    endpos = &buffer[1024];
  }
  std::string toString() {
    if (curpos != &buffer[0])
      data += buffer.substr(0, curpos - &buffer[0]);
    return data;
  }
  std::string buffer;
  std::string data;
};

int flush_func(apr_vformatter_buff_t* b) {
  apr_vformatter_buff_t_ex* vbufex = (apr_vformatter_buff_t_ex*)b;
  vbufex->data.append(vbufex->buffer);
  vbufex->reset();
  return 0;
}

std::string StringPrintV(_Printf_format_string_ const char* format, va_list ap) {
  apr_vformatter_buff_t_ex vbufex;
  apr_vformatter(flush_func, &vbufex, format, ap);
  return vbufex.toString();
}

}

std::string StringPrintf(_Printf_format_string_ const char* format,
  ...) {
  va_list ap;
  va_start(ap, format);
  return StringPrintV(format, ap);
}

void StringAppendF(std::string* dst, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  dst->append(StringPrintV(format, ap));
  va_end(ap);
}

bool StringToSizeT(const std::string& input, size_t* output) {
  apr_uint64_t n = 0;
  apr_status_t r = apr_cstr_atoui64(&n, input.c_str());
  if (r == APR_SUCCESS) {
    *output = (size_t)n;
    return true;
  }
  return false;
}

}