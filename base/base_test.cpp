#include "thread.h"
#include "apr_general.h"
#include <iostream>
#include "apr_init.h"

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
  MyThread thr;
  thr.Start();
  thr.Wait();
  return 0;
}