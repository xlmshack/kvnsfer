#include "event_loop.h"
#include <iostream>
#include <algorithm>
#include <map>
#include "base/apr_init.h"

class EventHandler : public EventLoop
                   , public EventLoop::Delegate {
public:
  EventHandler()
    :EventLoop(this) {

  }

  virtual ~EventHandler() {

  }

  virtual void OnConnect(apr_uint32_t id) {
    std::cout << "connected to [" << id << "]" << std::endl;
    std::string data = "hello world";
    Write(id, data.c_str(), data.size());
    std::cout << "wrote into [" << id << "]" << std::endl;
  }

  virtual void OnAccept(apr_uint32_t id, const std::string& addr, apr_uint16_t port) {
    std::cout << "accepted to [" << id << "] " << addr << ':' << port << std::endl;
  }

  virtual void OnRead(apr_uint32_t id, const char* buffer, apr_uint32_t size) {
    std::cout << "read from [" << id << "] buffer:" << std::string(buffer, size) << std::endl;
  }

  virtual void OnWrite(apr_uint32_t id) {
    std::cout << "wrote complete at [" << id << "]" << std::endl;
  }

  virtual void OnClose(apr_uint32_t id) {
    std::cout << "closed at [" << id << "]" << std::endl;
  }
};

int main(int argc, char* argv[]) {
  base::EnsureAprInit();
  EventHandler handler;
  std::string addr = "0.0.0.0";
  uint16_t port = 0;
  handler.Listen(addr, &port, 1);
  handler.Start();
  apr_uint32_t id = handler.Connect("45.78.10.146", 7);
  handler.Wait();
  return 0;
}