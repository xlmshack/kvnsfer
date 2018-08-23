#ifndef KVNSFER_BASE_MUTEX_H_
#define KVNSFER_BASE_MUTEX_H_

struct apr_thread_mutex_t;
struct apr_pool_t;

namespace base {

class Mutex {
public:
  Mutex();
  ~Mutex();

  void Lock();
  void Unlock();

protected:
  apr_thread_mutex_t* get_mutex() { return mutex_; }

private:
  apr_thread_mutex_t* mutex_;
  apr_pool_t* pool_;
  friend class Condition;

  Mutex(const Mutex&) = delete;
  Mutex& operator=(const Mutex&) = delete;
};

class MutexLockGuard {
public:
  explicit MutexLockGuard(Mutex& mutex);
  ~MutexLockGuard();

private:
  Mutex& mutex_;

  MutexLockGuard(const MutexLockGuard&) = delete;
  MutexLockGuard& operator=(const MutexLockGuard&) = delete;
};

} // namespace base

#endif // KVNSFER_BASE_MUTEX_H_
