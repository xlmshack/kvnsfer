#include "http_server_request_info.h"
#include <algorithm>
#include <wx/defs.h>
#include "string_util.h"

HttpServerRequestInfo::HttpServerRequestInfo() {}

HttpServerRequestInfo::HttpServerRequestInfo(
  const HttpServerRequestInfo& other) = default;

HttpServerRequestInfo::~HttpServerRequestInfo() {}

std::string HttpServerRequestInfo::GetHeaderValue(
  const std::string& header_name) const {
  wxASSERT(base::ToLowerASCII(header_name) == header_name);
  HttpServerRequestInfo::HeadersMap::const_iterator it =
    headers.find(header_name);
  if (it != headers.end())
    return it->second;
  return std::string();
}

bool HttpServerRequestInfo::HasHeaderValue(
  const std::string& header_name,
  const std::string& header_value) const {
  wxASSERT(base::ToLowerASCII(header_value) == header_value);
  std::string complete_value = base::ToLowerASCII(GetHeaderValue(header_name));

  // TODO(elvis) replace code below with std::string.
  wxString wx_complete_value(complete_value.c_str(), complete_value.size());
  wxArrayString split_res = wxSplit(wx_complete_value, ',');
  for (auto itcur = split_res.begin(); itcur != split_res.end(); ++itcur) {
    if (itcur->Trim().Trim(false) == 
      wxString(header_value.c_str(), header_value.size()))
      return true;
  }
  return false;
}