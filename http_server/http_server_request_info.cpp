#include "http_server_request_info.h"

HttpServerRequestInfo::HttpServerRequestInfo() {

}

HttpServerRequestInfo::HttpServerRequestInfo(const HttpServerRequestInfo& other) {

}

HttpServerRequestInfo::~HttpServerRequestInfo() {

}

std::string HttpServerRequestInfo::GetHeaderValue(const std::string& header_name) const {

}

bool HttpServerRequestInfo::HasHeaderValue(
  const std::string& header_name,
  const std::string& header_value) const {

}