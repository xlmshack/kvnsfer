#ifndef KVNSFER_HTTP_SERVER_HTTP_CONNECTION_H_
#define KVNSFER_HTTP_SERVER_HTTP_CONNECTION_H_

#include <wx/defs.h>
#include <wx/buffer.h>

class HttpConnection {
public:
  HttpConnection(wxUint32 id);
  ~HttpConnection();

  wxUint32 GetId();
  wxMemoryBuffer& GetReadBuf();
  wxMemoryBuffer& GetWriteBuf();

private:
  const wxUint32 id_;
  wxMemoryBuffer read_buf_;
  wxMemoryBuffer write_buf_;
};
#endif // KVNSFER_HTTP_SERVER_HTTP_CONNECTION_H_
