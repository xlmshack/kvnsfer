#include "event_loop.h"
#include <wx/init.h>
#include <wx/socket.h>
#include <iostream>

class EventHandler : public EventLoop::Delegate {
public:
  virtual void OnConnect(wxUint32 id) {
    std::cout << "connect to " << id << std::endl;
  }

  virtual void OnRead(wxUint32 id, wxSocketInputStream& stream) {
    char buffer[1024] = { 0 };
    stream.Read(buffer, sizeof(buffer));
    wxUint32 rdcnt = stream.LastRead();
    if (rdcnt > 0) {
      buffer_.append(buffer);
    }
  }

  virtual void OnWrite(wxUint32 id, wxSocketOutputStream& stream) {
    while (true) {
      size_t pos = buffer_.find('\n');
      if (pos != std::string::npos) {
        stream.Write(buffer_.c_str(), pos + 1);
        buffer_.erase(0, pos + 1);
      }
      else {
        break;
      }
    }
  }

  virtual void OnClose(wxUint32 id) {
    std::cout << "closed to " << id << std::endl;
  }

private:
  std::string buffer_;
};

int main(int argc, char* argv[]) {
  wxInitialize();
  wxIPV4address addr;
  addr.Hostname("127.0.0.1");
  addr.Service(8080);
  EventHandler handler;
  EventLoop loop(addr, &handler);
  loop.Run();
  loop.Wait();
  wxUninitialize();
  return 0;
}