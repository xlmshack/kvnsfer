#ifndef KVNSFER_TRANSFER_CORE_TRANSFER_TASK_H_
#define KVNSFER_TRANSFER_CORE_TRANSFER_TASK_H_

class TransferTask {
public:
  TransferTask();
  virtual ~TransferTask();
  virtual void Start() = 0;
  virtual void Cancel() = 0;

  TransferTask(const TransferTask&) = delete;
  TransferTask& operator=(const TransferTask&) = delete;
};

#endif // KVNSFER_TRANSFER_CORE_TRANSFER_TASK_H_
