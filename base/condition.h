#ifndef KVNSFER_BASE_CONDITION_H_
#define KVNSFER_BASE_CONDITION_H_

struct apr_thread_cond_t;
struct apr_pool_t;

namespace base {

class Mutex;

class Condition {
public:
  explicit Condition(Mutex& mutex);
  ~Condition();
  void Wait();
  void Notify();
  void NotifyAll();

private:
  Mutex& mutex_;
  apr_thread_cond_t* cond_;
  apr_pool_t* pool_;

  Condition(const Condition&) = delete;
  Condition& operator=(const Condition&) = delete;
};

} // namespace base

#endif // KVNSFER_BASE_CONDITION_H_
