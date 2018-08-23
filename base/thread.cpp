#include "thread.h"
#include <assert.h>
#include <apr_pools.h>
#include <apr_thread_proc.h>

namespace base {

Thread::Thread()
  :pool_(nullptr)
  , thread_(nullptr) {
}

Thread::~Thread() {
  if (pool_)
    apr_pool_destroy(pool_);
}

void Thread::Start() {
  apr_status_t status = APR_SUCCESS;
  status = apr_pool_create(&pool_, nullptr);
  assert(status == APR_SUCCESS);
  apr_threadattr_t* threadattr;
  status = apr_threadattr_create(&threadattr, pool_);
  assert(status == APR_SUCCESS);
  status = apr_thread_create(&thread_, threadattr, Thread::thread_start_func, this, pool_);
  assert(status == APR_SUCCESS);
}

void Thread::Wait() {
  assert(thread_);
  apr_status_t status = APR_SUCCESS;
  apr_status_t retval = APR_SUCCESS;
  status = apr_thread_join(&retval, thread_);
}

void* Thread::thread_start_func(apr_thread_t* thread, void* arg) {
  Thread* cur = (Thread*)arg;
  assert(cur);
  cur->Entry();
  return nullptr;
}

} // namespace base