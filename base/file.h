#ifndef KVNSFER_BASE_FILE_H_
#define KVNSFER_BASE_FILE_H_

#include <apr.h>
#include <apr_file_io.h>

struct apr_pool_t;
struct apr_file_t;

namespace base {

class File {
public:
  File();
  File(const char* fname, apr_int32_t flag);
  virtual ~File();

  void Open(const char* fname, apr_int32_t flag);
  void Close();
  void Flush();
  void Seek(apr_seek_where_t where, apr_off_t *offset);
  void Read(void* buf, apr_size_t *nbytes);
  void Write(const void *buf, apr_size_t *nbytes);

private:
  apr_pool_t* pool_;
  apr_file_t* file_;

  File(const File&) = delete;
  File& operator=(const File&) = delete;
};

} // namespace base

#endif // KVNSFER_BASE_FILE_H_
