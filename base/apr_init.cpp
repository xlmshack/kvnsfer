#include "apr_init.h"
#include <apr_general.h>

namespace base {

class AprInitSingleton {
public:
  AprInitSingleton() {
    apr_initialize();
  }

  static void Get() {
    static AprInitSingleton* instance = nullptr;
    if (!instance) {
      instance = new AprInitSingleton();
    }
  }
};

void EnsureAprInit() {
  AprInitSingleton::Get();
}

} // namespace base