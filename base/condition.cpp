#include "condition.h"

#include <assert.h>

#include "apr.h"
#include "apr_general.h"
#include "apr_errno.h"
#include "apr_thread_cond.h"

#include "mutex.h"

namespace base {

Condition::Condition(Mutex& mutex)
  :mutex_(mutex) {
  apr_status_t status = APR_SUCCESS;
  status = apr_pool_create(&pool_, NULL);
  assert(status == APR_SUCCESS);
  status = apr_thread_cond_create(&cond_, pool_);
  assert(status == APR_SUCCESS);
}

Condition::~Condition() {
  apr_status_t status;
  status = apr_thread_cond_destroy(cond_);
  assert(status == APR_SUCCESS);
  apr_pool_destroy(pool_);
}

void Condition::Wait() {
  apr_thread_cond_wait(cond_, mutex_.get_mutex());
}

void Condition::Notify() {
  apr_thread_cond_signal(cond_);
}

void Condition::NotifyAll() {
  apr_thread_cond_broadcast(cond_);
}

} // namespace base