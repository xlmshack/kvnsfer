#ifndef KVNSFER_TRANSFER_CORE_TRANSFER_MANAGER_H_
#define KVNSFER_TRANSFER_CORE_TRANSFER_MANAGER_H_

#include <wx/defs.h>

class TransferManager {
public:
  TransferManager();
  virtual ~TransferManager();
  virtual void CreateTransferSendTask() = 0;
  virtual void CreateTransferReceiveTask() = 0;
  virtual void StartTransferTask() = 0;
  virtual void CancelTransferTask() = 0;
  virtual void RemoveTransferTask() = 0;

  wxDECLARE_NO_COPY_CLASS(TransferManager);
};
#endif // KVNSFER_TRANSFER_CORE_TRANSFER_MANAGER_H_
