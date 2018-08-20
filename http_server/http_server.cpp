#include "http_server.h"
#include "http_connection.h"
#include "http_server_request_info.h"
#include "string_util.h"

HttpServer::HttpServer(const std::string& vir_host)
  :event_loop_(this)
  , last_id_(0) {

}

HttpServer::~HttpServer() {

}

void HttpServer::StartServer(const std::string& addr, wxUint16* port) {
  event_loop_.Listen(addr, port, 1);
  event_loop_.Run();
}

void HttpServer::StopServer() {
  event_loop_.Exit();
  event_loop_.Wait();
}

void HttpServer::OnAccept(wxUint32 id, const std::string& addr, wxUint16 port) {
  std::unique_ptr<HttpConnection> conn = std::make_unique<HttpConnection>(++last_id_);
  id_to_connection_[conn->GetId()] = std::move(conn);
}

void HttpServer::OnRead(wxUint32 id, const char* buffer, wxUint32 size) {
  auto itconn = id_to_connection_.find(id);
  if (itconn == id_to_connection_.end()) {
    return;
  }
  HttpConnection& conn = *itconn->second;
  conn.GetReadBuf().AppendData(buffer, size);
}

void HttpServer::OnWrite(wxUint32 id) {

}

void HttpServer::OnClose(wxUint32 id) {

}

namespace {

  //
  // HTTP Request Parser
  // This HTTP request parser uses a simple state machine to quickly parse
  // through the headers.  The parser is not 100% complete, as it is designed
  // for use in this simple test driver.
  //
  // Known issues:
  //   - does not handle whitespace on first HTTP line correctly.  Expects
  //     a single space between the method/url and url/protocol.

  // Input character types.
  enum header_parse_inputs {
    INPUT_LWS,
    INPUT_CR,
    INPUT_LF,
    INPUT_COLON,
    INPUT_DEFAULT,
    MAX_INPUTS,
  };

  // Parser states.
  enum header_parse_states {
    ST_METHOD,     // Receiving the method
    ST_URL,        // Receiving the URL
    ST_PROTO,      // Receiving the protocol
    ST_HEADER,     // Starting a Request Header
    ST_NAME,       // Receiving a request header name
    ST_SEPARATOR,  // Receiving the separator between header name and value
    ST_VALUE,      // Receiving a request header value
    ST_DONE,       // Parsing is complete and successful
    ST_ERR,        // Parsing encountered invalid syntax.
    MAX_STATES
  };

  // State transition table
  const int parser_state[MAX_STATES][MAX_INPUTS] = {
    /* METHOD    */{ ST_URL, ST_ERR, ST_ERR, ST_ERR, ST_METHOD },
    /* URL       */{ ST_PROTO, ST_ERR, ST_ERR, ST_URL, ST_URL },
    /* PROTOCOL  */{ ST_ERR, ST_HEADER, ST_NAME, ST_ERR, ST_PROTO },
    /* HEADER    */{ ST_ERR, ST_ERR, ST_NAME, ST_ERR, ST_ERR },
    /* NAME      */{ ST_SEPARATOR, ST_DONE, ST_ERR, ST_VALUE, ST_NAME },
    /* SEPARATOR */{ ST_SEPARATOR, ST_ERR, ST_ERR, ST_VALUE, ST_ERR },
    /* VALUE     */{ ST_VALUE, ST_HEADER, ST_NAME, ST_VALUE, ST_VALUE },
    /* DONE      */{ ST_DONE, ST_DONE, ST_DONE, ST_DONE, ST_DONE },
    /* ERR       */{ ST_ERR, ST_ERR, ST_ERR, ST_ERR, ST_ERR } };

  // Convert an input character to the parser's input token.
  int charToInput(char ch) {
    switch (ch) {
    case ' ':
    case '\t':
      return INPUT_LWS;
    case '\r':
      return INPUT_CR;
    case '\n':
      return INPUT_LF;
    case ':':
      return INPUT_COLON;
    }
    return INPUT_DEFAULT;
  }

}  // namespace

bool HttpServer::ParseHeaders(const char* data, size_t data_len,
  HttpServerRequestInfo* info, size_t* ppos) {
  size_t& pos = *ppos;
  int state = ST_METHOD;
  std::string buffer;
  std::string header_name;
  std::string header_value;
  while (pos < data_len) {
    char ch = data[pos++];
    int input = charToInput(ch);
    int next_state = parser_state[state][input];

    bool transition = (next_state != state);
    HttpServerRequestInfo::HeadersMap::iterator it;
    if (transition) {
      // Do any actions based on state transitions.
      switch (state) {
      case ST_METHOD:
        info->method = buffer;
        buffer.clear();
        break;
      case ST_URL:
        info->path = buffer;
        buffer.clear();
        break;
      case ST_PROTO:
        if (buffer != "HTTP/1.1") {
          //LOG(ERROR) << "Cannot handle request with protocol: " << buffer;
          next_state = ST_ERR;
        }
        buffer.clear();
        break;
      case ST_NAME:
        header_name = base::ToLowerASCII(buffer);
        buffer.clear();
        break;
      case ST_VALUE:
        header_value = base::TrimWhitespace(buffer, base::TRIM_LEADING);
        it = info->headers.find(header_name);
        // See the second paragraph ("A sender MUST NOT generate multiple
        // header fields...") of tools.ietf.org/html/rfc7230#section-3.2.2.
        if (it == info->headers.end()) {
          info->headers[header_name] = header_value;
        }
        else {
          it->second.append(",");
          it->second.append(header_value);
        }
        buffer.clear();
        break;
      case ST_SEPARATOR:
        break;
      }
      state = next_state;
    }
    else {
      // Do any actions based on current state
      switch (state) {
      case ST_METHOD:
      case ST_URL:
      case ST_PROTO:
      case ST_VALUE:
      case ST_NAME:
        buffer.append(&ch, 1);
        break;
      case ST_DONE:
        // We got CR to get this far, also need the LF
        return (input == INPUT_LF);
      case ST_ERR:
        return false;
      }
    }
  }
  // No more characters, but we haven't finished parsing yet. Signal this to
  // the caller by setting |pos| to zero.
  pos = 0;
  return true;
}