#include "event_loop.h"
#include <wx/init.h>
#include <wx/socket.h>
#include <iostream>
#include <algorithm>
#include <map>

class EventHandler : public EventLoop
                   , public EventLoop::Delegate {
public:
  EventHandler(wxIPaddress& addr)
    :EventLoop(addr, this) {

  }

  virtual ~EventHandler() {

  }

  virtual void OnConnect(wxUint32 id) {

  }

  virtual void OnAccept(wxUint32 id) {
    std::cout << "accept to " << id << std::endl;
    buffers_[id];
  }

  virtual void OnRead(wxUint32 id, wxInputStream& stream) {
    char buffer[1024] = { 0 };
    stream.Read(buffer, sizeof(buffer));
    wxUint32 rdcnt = stream.LastRead();      
    if (rdcnt > 0) {
      buffers_.find(id)->second.append(buffer);
    }
    size_t pos = buffers_.find(id)->second.find('\n');
    if (pos != std::string::npos) {
      this->SetNeedWrite(id);
    }
  }

  virtual void OnWrite(wxUint32 id, wxOutputStream& stream) {
    size_t pos = buffers_.find(id)->second.find('\n');
    if (pos != std::string::npos) {
      std::string line = buffers_.find(id)->second.substr(0, pos + 1);
      std::transform(line.begin(), line.end(), line.begin(), toupper);
      stream.Write(line.c_str(), line.size());
      buffers_.find(id)->second.erase(0, pos + 1);
    }
  }

  virtual void OnClose(wxUint32 id) {
    std::cout << "closed to " << id << std::endl;
    buffers_.erase(id);
  }

private:
  std::map<wxUint32, std::string> buffers_;
};

int main(int argc, char* argv[]) {
  wxInitialize();
  wxIPV4address addr;
  addr.Hostname("127.0.0.1");
  addr.Service(8080);
  EventHandler handler(addr);
  handler.Run();
  handler.Wait();
  wxUninitialize();
  return 0;
}