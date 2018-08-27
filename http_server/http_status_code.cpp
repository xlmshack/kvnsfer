#include "http_status_code.h"
#include <assert.h>
const char* GetHttpReasonPhrase(HttpStatusCode code) {
  switch (code) {

#define HTTP_STATUS(label, code, reason) case HTTP_ ## label: return reason;
#include "http_status_code_list.h"
#undef HTTP_STATUS

  default:
    assert(false);

  }

  return "";
}
