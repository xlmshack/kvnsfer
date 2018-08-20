#include "http_request_headers.h"

#include <utility>
#include <assert.h>
#include <wx/log.h>

#include "http_util.h"
#include "string_util.h"
#include "string_split.h"

const char HttpRequestHeaders::kGetMethod[] = "GET";
const char HttpRequestHeaders::kAcceptCharset[] = "Accept-Charset";
const char HttpRequestHeaders::kAcceptEncoding[] = "Accept-Encoding";
const char HttpRequestHeaders::kAcceptLanguage[] = "Accept-Language";
const char HttpRequestHeaders::kAuthorization[] = "Authorization";
const char HttpRequestHeaders::kCacheControl[] = "Cache-Control";
const char HttpRequestHeaders::kConnection[] = "Connection";
const char HttpRequestHeaders::kContentLength[] = "Content-Length";
const char HttpRequestHeaders::kContentType[] = "Content-Type";
const char HttpRequestHeaders::kCookie[] = "Cookie";
const char HttpRequestHeaders::kHost[] = "Host";
const char HttpRequestHeaders::kIfMatch[] = "If-Match";
const char HttpRequestHeaders::kIfModifiedSince[] = "If-Modified-Since";
const char HttpRequestHeaders::kIfNoneMatch[] = "If-None-Match";
const char HttpRequestHeaders::kIfRange[] = "If-Range";
const char HttpRequestHeaders::kIfUnmodifiedSince[] = "If-Unmodified-Since";
const char HttpRequestHeaders::kOrigin[] = "Origin";
const char HttpRequestHeaders::kPragma[] = "Pragma";
const char HttpRequestHeaders::kProxyAuthorization[] = "Proxy-Authorization";
const char HttpRequestHeaders::kProxyConnection[] = "Proxy-Connection";
const char HttpRequestHeaders::kRange[] = "Range";
const char HttpRequestHeaders::kReferer[] = "Referer";
const char HttpRequestHeaders::kTransferEncoding[] = "Transfer-Encoding";
const char HttpRequestHeaders::kTokenBinding[] = "Sec-Token-Binding";
const char HttpRequestHeaders::kUserAgent[] = "User-Agent";

HttpRequestHeaders::HeaderKeyValuePair::HeaderKeyValuePair() {
}

HttpRequestHeaders::HeaderKeyValuePair::HeaderKeyValuePair(
  const std::string& key, const std::string& value)
  : key(key.data(), key.size()), value(value.data(), value.size()) {
}


HttpRequestHeaders::Iterator::Iterator(const HttpRequestHeaders& headers)
  : started_(false),
  curr_(headers.headers_.begin()),
  end_(headers.headers_.end()) {}

HttpRequestHeaders::Iterator::~Iterator() {}

bool HttpRequestHeaders::Iterator::GetNext() {
  if (!started_) {
    started_ = true;
    return curr_ != end_;
  }

  if (curr_ == end_)
    return false;

  ++curr_;
  return curr_ != end_;
}

HttpRequestHeaders::HttpRequestHeaders() {}
HttpRequestHeaders::HttpRequestHeaders(const HttpRequestHeaders& other) =
  default;
HttpRequestHeaders::~HttpRequestHeaders() {}

bool HttpRequestHeaders::GetHeader(const std::string& key,
  std::string* out) const {
  HeaderVector::const_iterator it = FindHeader(key);
  if (it == headers_.end())
    return false;
  out->assign(it->value);
  return true;
}

void HttpRequestHeaders::Clear() {
  headers_.clear();
}

void HttpRequestHeaders::SetHeader(const std::string& key,
  const std::string& value) {
  assert(HttpUtil::IsValidHeaderName(key));
  assert(HttpUtil::IsValidHeaderValue(value));
  HeaderVector::iterator it = FindHeader(key);
  if (it != headers_.end())
    it->value.assign(value.data(), value.size());
  else
    headers_.push_back(HeaderKeyValuePair(key, value));
}

void HttpRequestHeaders::SetHeaderIfMissing(const std::string& key,
  const std::string& value) {
  assert(HttpUtil::IsValidHeaderName(key));
  assert(HttpUtil::IsValidHeaderValue(value));
  HeaderVector::iterator it = FindHeader(key);
  if (it == headers_.end())
    headers_.push_back(HeaderKeyValuePair(key, value));
}

void HttpRequestHeaders::RemoveHeader(const std::string& key) {
  HeaderVector::iterator it = FindHeader(key);
  if (it != headers_.end())
    headers_.erase(it);
}

void HttpRequestHeaders::AddHeaderFromString(
  const std::string& header_line) {
  assert(std::string::npos == header_line.find("\r\n"));

  const std::string::size_type key_end_index = header_line.find(":");
  if (key_end_index == std::string::npos) {
    //LOG(DFATAL) << "\"" << header_line << "\" is missing colon delimiter.";
    return;
  }

  if (key_end_index == 0) {
    //LOG(DFATAL) << "\"" << header_line << "\" is missing header key.";
    return;
  }

  const std::string header_key(header_line.data(), key_end_index);
  if (!HttpUtil::IsValidHeaderName(header_key)) {
    //LOG(DFATAL) << "\"" << header_line << "\" has invalid header key.";
    return;
  }

  const std::string::size_type value_index = key_end_index + 1;

  if (value_index < header_line.size()) {
    std::string header_value(header_line.data() + value_index,
      header_line.size() - value_index);
    header_value = HttpUtil::TrimLWS(header_value);
    if (!HttpUtil::IsValidHeaderValue(header_value)) {
      //LOG(DFATAL) << "\"" << header_line << "\" has invalid header value.";
      return;
    }
    SetHeader(header_key, header_value);
  }
  else if (value_index == header_line.size()) {
    SetHeader(header_key, "");
  }
  else {
    //NOTREACHED();
  }
}

void HttpRequestHeaders::AddHeadersFromString(
  const std::string& headers) {
  for (const std::string& header : base::SplitString(
    headers, "\r\n", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY)) {
    AddHeaderFromString(header);
  }
}

void HttpRequestHeaders::MergeFrom(const HttpRequestHeaders& other) {
  for (HeaderVector::const_iterator it = other.headers_.begin();
    it != other.headers_.end(); ++it) {
    SetHeader(it->key, it->value);
  }
}

std::string HttpRequestHeaders::ToString() const {
  std::string output;
  for (HeaderVector::const_iterator it = headers_.begin();
    it != headers_.end(); ++it) {
    if (!it->value.empty()) {
      base::StringAppendF(&output, "%s: %s\r\n",
        it->key.c_str(), it->value.c_str());
    }
    else {
      base::StringAppendF(&output, "%s:\r\n", it->key.c_str());
    }
  }
  output.append("\r\n");
  return output;
}

HttpRequestHeaders::HeaderVector::iterator
  HttpRequestHeaders::FindHeader(const std::string& key) {
  for (HeaderVector::iterator it = headers_.begin();
    it != headers_.end(); ++it) {
    if (base::EqualsCaseInsensitiveASCII(key, it->key))
      return it;
  }

  return headers_.end();
}

HttpRequestHeaders::HeaderVector::const_iterator
  HttpRequestHeaders::FindHeader(const std::string& key) const {
  for (HeaderVector::const_iterator it = headers_.begin();
    it != headers_.end(); ++it) {
    if (base::EqualsCaseInsensitiveASCII(key, it->key))
      return it;
  }

  return headers_.end();
}