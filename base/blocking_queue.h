#ifndef KVNSFER_BASE_BLOCKING_QUEUE_H_
#define KVNSFER_BASE_BLOCKING_QUEUE_H_

#include <list>
#include "mutex.h"
#include "condition.h"

namespace base {

template<class T>
class BlockingQueue {
public:
  BlockingQueue();
  ~BlockingQueue();

  void Push(T& elt);
  T Pop();

private:
  std::list<T> elt_list_;
  Mutex mutex_;
  Condition cond_;
};

} // namespace base

#endif // KVNSFER_BASE_BLOCKING_QUEUE_H_
