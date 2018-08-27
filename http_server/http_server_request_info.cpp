#include "http_server_request_info.h"
#include <algorithm>
#include "base/string_util.h"
#include <assert.h>
#include "base/string_split.h"

HttpServerRequestInfo::HttpServerRequestInfo() {}

HttpServerRequestInfo::HttpServerRequestInfo(
  const HttpServerRequestInfo& other) = default;

HttpServerRequestInfo::~HttpServerRequestInfo() {}

std::string HttpServerRequestInfo::GetHeaderValue(
  const std::string& header_name) const {
  assert(base::ToLowerASCII(header_name) == header_name);
  HttpServerRequestInfo::HeadersMap::const_iterator it =
    headers.find(header_name);
  if (it != headers.end())
    return it->second;
  return std::string();
}

bool HttpServerRequestInfo::HasHeaderValue(
  const std::string& header_name,
  const std::string& header_value) const {
  assert(base::ToLowerASCII(header_value) == header_value);
  std::string complete_value = base::ToLowerASCII(GetHeaderValue(header_name));

  for (const std::string& cur :
    base::SplitString(complete_value, ",", base::KEEP_WHITESPACE,
      base::SPLIT_WANT_NONEMPTY)) {
    if (base::TrimString(cur, " \t", base::TRIM_ALL) == header_value)
      return true;
  }
  return false;
}