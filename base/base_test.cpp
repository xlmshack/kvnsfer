#include "thread.h"
#include "apr_general.h"
#include <iostream>

class MyThread : public base::Thread {
public:
  MyThread() {}
  ~MyThread() override {}

  void Entry() override {
    std::cout << "hello world" << std::endl;
  }
};

int main(int argc, char *argv[]) {
  apr_initialize();
  {
    MyThread thr;
    thr.Start();
    thr.Wait();
  }
  apr_terminate();
  return 0;
}