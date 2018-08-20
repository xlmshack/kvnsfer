#ifndef KVNSFER_HTTP_SERVER_HTTP_REQUEST_HEADERS_H_
#define KVNSFER_HTTP_SERVER_HTTP_REQUEST_HEADERS_H_

#include <memory>
#include <string>
#include <vector>
#include <wx/defs.h>

class HttpRequestHeaders {
public:
  struct HeaderKeyValuePair {
    HeaderKeyValuePair();
    HeaderKeyValuePair(const std::string& key,
      const std::string& value);

    std::string key;
    std::string value;
  };

  typedef std::vector<HeaderKeyValuePair> HeaderVector;

  class Iterator {
  public:
    explicit Iterator(const HttpRequestHeaders& headers);
    ~Iterator();

    // Advances the iterator to the next header, if any.  Returns true if there
    // is a next header.  Use name() and value() methods to access the resultant
    // header name and value.
    bool GetNext();

    // These two accessors are only valid if GetNext() returned true.
    const std::string& name() const { return curr_->key; }
    const std::string& value() const { return curr_->value; }

  private:
    bool started_;
    HttpRequestHeaders::HeaderVector::const_iterator curr_;
    const HttpRequestHeaders::HeaderVector::const_iterator end_;

    wxDECLARE_NO_COPY_CLASS(Iterator);
  };

  static const char kGetMethod[];

  static const char kAcceptCharset[];
  static const char kAcceptEncoding[];
  static const char kAcceptLanguage[];
  static const char kAuthorization[];
  static const char kCacheControl[];
  static const char kConnection[];
  static const char kContentType[];
  static const char kCookie[];
  static const char kContentLength[];
  static const char kHost[];
  static const char kIfMatch[];
  static const char kIfModifiedSince[];
  static const char kIfNoneMatch[];
  static const char kIfRange[];
  static const char kIfUnmodifiedSince[];
  static const char kOrigin[];
  static const char kPragma[];
  static const char kProxyAuthorization[];
  static const char kProxyConnection[];
  static const char kRange[];
  static const char kReferer[];
  static const char kTransferEncoding[];
  static const char kTokenBinding[];
  static const char kUserAgent[];

  HttpRequestHeaders();
  HttpRequestHeaders(const HttpRequestHeaders& other);
  ~HttpRequestHeaders();

  bool IsEmpty() const { return headers_.empty(); }

  bool HasHeader(const std::string& key) const {
    return FindHeader(key) != headers_.end();
  }

  // Gets the first header that matches |key|.  If found, returns true and
  // writes the value to |out|.
  bool GetHeader(const std::string& key, std::string* out) const;

  // Clears all the headers.
  void Clear();

  // Sets the header value pair for |key| and |value|.  If |key| already exists,
  // then the header value is modified, but the key is untouched, and the order
  // in the vector remains the same.  When comparing |key|, case is ignored.
  // The caller must ensure that |key| passes HttpUtil::IsValidHeaderName() and
  // |value| passes HttpUtil::IsValidHeaderValue().
  void SetHeader(const std::string& key, const std::string& value);

  // Sets the header value pair for |key| and |value|, if |key| does not exist.
  // If |key| already exists, the call is a no-op.
  // When comparing |key|, case is ignored.
  void SetHeaderIfMissing(const std::string& key,
    const std::string& value);

  // Removes the first header that matches (case insensitive) |key|.
  void RemoveHeader(const std::string& key);

  // Parses the header from a string and calls SetHeader() with it.  This string
  // should not contain any CRLF.  As per RFC7230 Section 3.2, the format is:
  //
  // header-field   = field-name ":" OWS field-value OWS
  //
  // field-name     = token
  // field-value    = *( field-content / obs-fold )
  // field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
  // field-vchar    = VCHAR / obs-text
  //
  // obs-fold       = CRLF 1*( SP / HTAB )
  //                ; obsolete line folding
  //                ; see Section 3.2.4
  //
  // AddHeaderFromString() will trim any LWS surrounding the
  // field-content.
  void AddHeaderFromString(const std::string& header_line);

  // Same thing as AddHeaderFromString() except that |headers| is a "\r\n"
  // delimited string of header lines.  It will split up the string by "\r\n"
  // and call AddHeaderFromString() on each.
  void AddHeadersFromString(const std::string& headers);

  // Calls SetHeader() on each header from |other|, maintaining order.
  void MergeFrom(const HttpRequestHeaders& other);

  // Copies from |other| to |this|.
  void CopyFrom(const HttpRequestHeaders& other) {
    *this = other;
  }

  void Swap(HttpRequestHeaders* other) {
    headers_.swap(other->headers_);
  }

  // Serializes HttpRequestHeaders to a string representation.  Joins all the
  // header keys and values with ": ", and inserts "\r\n" between each header
  // line, and adds the trailing "\r\n".
  std::string ToString() const;

private:
  HeaderVector::iterator FindHeader(const std::string& key);
  HeaderVector::const_iterator FindHeader(const std::string& key) const;

  HeaderVector headers_;

  // Allow the copy construction and operator= to facilitate copying in
  // HttpRequestHeaders.
  // TODO(willchan): Investigate to see if we can remove the need to copy
  // HttpRequestHeaders.
  // DISALLOW_COPY_AND_ASSIGN(HttpRequestHeaders);
};

#endif  // KVNSFER_HTTP_SERVER_HTTP_REQUEST_HEADERS_H_
