#include "event_loop.h"
#include <wx/init.h>
#include <wx/socket.h>
#include <iostream>

class EventHandler : public EventLoop::Delegate {
  virtual void OnConnect(wxUint32 id) {

  }

  virtual void OnRead(wxUint32 id, wxSocketInputStream& stream) {
    std::string buf(1024, 0);
    stream.Read((void*)(buf.c_str()), buf.size());
    wxInt32 rdcnt = stream.LastRead();
    buf.resize(rdcnt);
    std::cout << buf << std::endl;
  }

  virtual void OnWrite(wxUint32 id, wxSocketOutputStream& stream) {
    std::string buf = "hello world";
    //stream.Write(buf.c_str(), buf.size());
  }

  virtual void OnClose(wxUint32 id) {

  }
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