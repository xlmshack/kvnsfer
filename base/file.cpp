#include "file.h"
#include <assert.h>

#include "apr.h"
#include "apr_general.h"
#include "apr_errno.h"

namespace base {

File::File()
  :file_(nullptr) {
  apr_status_t status = APR_SUCCESS;
  status = apr_pool_create(&pool_, NULL);
  assert(status == APR_SUCCESS);
}

File::File(const char* fname, apr_int32_t flag)
  :File() {
  Open(fname, flag);
}

File::~File() {
  if (file_) {
    apr_file_close(file_);
  }
  if (pool_) {
    apr_pool_destroy(pool_);
  }
}

void File::Open(const char* fname, apr_int32_t flag) {
  assert(file_ == nullptr);
  apr_status_t status = APR_SUCCESS;
  apr_fileperms_t perms = APR_FPROT_OS_DEFAULT;
  status = apr_file_open(&file_, fname, flag, perms, pool_);
}

void File::Close() {
  assert(file_);
  apr_file_close(file_);
  file_ = nullptr;
}

void File::Flush() {
  assert(file_);
  apr_file_flush(file_);
}

void File::Seek(apr_seek_where_t where, apr_off_t *offset) {
  assert(file_);
  apr_file_seek(file_, where, offset);
}

void File::Read(void* buf, apr_size_t *nbytes) {
  assert(file_);
  apr_file_read(file_, buf, nbytes);
}

void File::Write(const void *buf, apr_size_t *nbytes) {
  assert(file_);
  apr_file_write(file_, buf, nbytes);
}

} // namespace base