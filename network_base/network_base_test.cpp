#include "event_loop.h"
#include <wx/init.h>
#include <wx/socket.h>
#include <iostream>
#include <algorithm>
#include <map>

class EventHandler : public EventLoop
                   , public EventLoop::Delegate {
public:
  EventHandler()
    :EventLoop(this) {

  }

  virtual ~EventHandler() {

  }

  virtual void OnConnect(wxUint32 id) {
    std::cout << "connected to [" << id << "]" << std::endl;
    std::string data = "hello world";
    Write(id, data.c_str(), data.size());
    std::cout << "wrote into [" << id << "]" << std::endl;
  }

  virtual void OnAccept(wxUint32 id, const std::string& addr, wxUint16 port) {
    std::cout << "accepted to [" << id << "] " << addr << ':' << port << std::endl;
  }

  virtual void OnRead(wxUint32 id, const char* buffer, wxUint32 size) {
    std::cout << "read from [" << id << "] buffer:" << std::string(buffer, size) << std::endl;
  }

  virtual void OnWrite(wxUint32 id) {
    std::cout << "wrote complete at [" << id << "]" << std::endl;
  }

  virtual void OnClose(wxUint32 id) {
    std::cout << "closed at [" << id << "]" << std::endl;
  }
};

int main(int argc, char* argv[]) {
  wxInitialize();
  EventHandler handler;
  std::string addr = "0.0.0.0";
  uint16_t port = 0;
  handler.Listen(addr, &port, 1);
  handler.Run();
  wxUint32 id = handler.Connect("45.78.10.146", 7);
  handler.Wait();
  wxUninitialize();
  return 0;
}