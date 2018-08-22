#ifndef KVNSFER_BASE_THREAD_H_
#define KVNSFER_BASE_THREAD_H_

#include <apr.h>

struct apr_pool_t;
struct apr_thread_t;

namespace base {

class Thread {
public:
  Thread();
  virtual ~Thread();

  void Start();
  void Wait();

  virtual void Entry() = 0;

protected:
  static void* APR_THREAD_FUNC thread_start_func(apr_thread_t *, void *);

private:
  apr_pool_t* pool_;
  apr_thread_t* thread_;
};

} // namespace base

#endif // KVNSFER_BASE_THREAD_H_
