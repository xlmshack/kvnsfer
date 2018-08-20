#include "http_status_code.h"
#include "wx/defs.h"
#include <wx/string.h>

const char* GetHttpReasonPhrase(HttpStatusCode code) {
  switch (code) {

#define HTTP_STATUS(label, code, reason) case HTTP_ ## label: return reason;
#include "http_status_code_list.h"
#undef HTTP_STATUS

  default:
    wxASSERT_MSG(false, (wxString::Format("unknown HTTP status code %d", code).c_str()));

  }

  return "";
}
