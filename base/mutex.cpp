#include "mutex.h"
#include <assert.h>
#include <apr_errno.h>
#include <apr_thread_mutex.h>

namespace base {

Mutex::Mutex() {
  apr_status_t status = APR_SUCCESS;
  status = apr_pool_create(&pool_, NULL);
  assert(status == APR_SUCCESS);
  status = apr_thread_mutex_create(&mutex_, APR_THREAD_MUTEX_DEFAULT, pool_);
  assert(status == APR_SUCCESS);
}

Mutex::~Mutex() {
  apr_status_t status = APR_SUCCESS;
  status = apr_thread_mutex_destroy(mutex_);
  assert(status == APR_SUCCESS);
  apr_pool_destroy(pool_);
}

void Mutex::Lock() {
  apr_status_t status = APR_SUCCESS;
  status = apr_thread_mutex_lock(mutex_);
  assert(status == APR_SUCCESS);
}

void Mutex::Unlock() {
  apr_status_t status = APR_SUCCESS;
  status = apr_thread_mutex_unlock(mutex_);
  assert(status == APR_SUCCESS);
}

MutexLockGuard::MutexLockGuard(Mutex& mutex)
  :mutex_(mutex) {
  mutex_.Lock();
}

MutexLockGuard::~MutexLockGuard() {
  mutex_.Unlock();
}

#define MutexLockGuard(x) error "Missing guard object name"

} // namespace base