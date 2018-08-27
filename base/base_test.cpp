#include "thread.h"
#include "apr_general.h"
#include <iostream>
#include "apr_init.h"
#include "string_util.h"
#include "base/file.h"

class MyThread : public base::Thread {
public:
  MyThread() {}
  ~MyThread() override {}

  void Entry() override {
    std::cout << "hello world" << std::endl;
  }
};

int main(int argc, char *argv[]) {
  base::EnsureAprInit();
  base::File file("5mb.txt", APR_FOPEN_WRITE | APR_FOPEN_CREATE);
  for (apr_uint32_t i = 0; i < 1024 * 1024 * 5; i++) {
    char ch = '5';
    apr_size_t nbytes = 1;
    file.Write(&ch, &nbytes);
  }
  return 0;
}