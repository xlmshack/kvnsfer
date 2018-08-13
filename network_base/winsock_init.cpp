#include "winsock_init.h"
#include <wx/defs.h>

class WinsockInitSingleton {
public:
  WinsockInitSingleton() {
    WORD winsock_ver = MAKEWORD(2, 2);
    WSAData wsa_data;
    bool did_init = (WSAStartup(winsock_ver, &wsa_data) == 0);
    if (did_init) {
      wxASSERT(wsa_data.wVersion == winsock_ver);

      // The first time WSAGetLastError is called, the delay load helper will
      // resolve the address with GetProcAddress and fixup the import.  If a
      // third party application hooks system functions without correctly
      // restoring the error code, it is possible that the error code will be
      // overwritten during delay load resolution.  The result of the first
      // call may be incorrect, so make sure the function is bound and future
      // results will be correct.
      WSAGetLastError();
    }
  }

  static void Get() {
    static WinsockInitSingleton* instance = nullptr;
    if (!instance) {
      instance = new WinsockInitSingleton();
    }
  }
};

void EnsureWinsockInit() {
  WinsockInitSingleton::Get();
}