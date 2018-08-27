#include "thread.h"
#include "apr_general.h"
#include <iostream>
#include "apr_init.h"
#include "string_util.h"

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
  std::string str = base::StringPrintf("hello world %04d-%02d-%02d %02d:%02d:%02d", 2018, 4, 26, 12, 13, 1);
  std::string size_str = "14294967295";
  size_t size = 0;
  bool ret = base::StringToSizeT(size_str, &size);
  return 0;
}