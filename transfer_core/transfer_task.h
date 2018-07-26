#ifndef KVNSFER_TRANSFER_CORE_TRANSFER_TASK_H_
#define KVNSFER_TRANSFER_CORE_TRANSFER_TASK_H_

#include <wx/defs.h>

class TransferTask {
public:
  TransferTask();
  virtual ~TransferTask();
  virtual void Start() = 0;
  virtual void Cancel() = 0;

  wxDECLARE_NO_COPY_CLASS(TransferTask);
};

#endif // KVNSFER_TRANSFER_CORE_TRANSFER_TASK_H_
