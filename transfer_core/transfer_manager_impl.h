#ifndef KVNSFER_TRANSFER_CORE_TRANSFER_MANAGER_IMPL_H_
#define KVNSFER_TRANSFER_CORE_TRANSFER_MANAGER_IMPL_H_

#include "transfer_manager.h"

class TransferManagerImpl : public TransferManager {
public:
  // ctor & dtor
  TransferManagerImpl();
  virtual ~TransferManagerImpl() override;
  // TransferManager
  virtual void CreateTransferSendTask() override;
  virtual void CreateTransferReceiveTask() override;
  virtual void StartTransferTask() override;
  virtual void CancelTransferTask() override;
  virtual void RemoveTransferTask() override;

  TransferManagerImpl(const TransferManagerImpl&) = delete;
  TransferManagerImpl& operator=(const TransferManagerImpl&) = delete;
};

#endif // KVNSFER_TRANSFER_CORE_TRANSFER_MANAGER_IMPL_H_
