#include "blocking_queue.h"

namespace base {

template<class T>
BlockingQueue<T>::BlockingQueue()
  :cond_(mutex_) {

}

template<class T>
BlockingQueue<T>::~BlockingQueue() {

}

template<class T>
void BlockingQueue<T>::Push(T& elt) {
  {
    MutexLockGuard guard(mutex_);
    elt_list_.push(elt);
  }
  cond_.NotifyAll();
}

template<class T>
T BlockingQueue<T>::Pop() {
  MutexLockGuard guard(mutex_);
  while (elt_list_.empty())
    cond_.Wait();
  T elt = elt_list_.front();
  elt_list_.pop_front();
  return elt;
}

} // namespace base