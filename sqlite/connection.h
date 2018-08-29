#ifndef KVNSFER_SQLITE_CONNECTION_H_
#define KVNSFER_SQLITE_CONNECTION_H_

#include "apr.h"
#include <memory>

struct sqlite3;

namespace sql {

class Connection {
public:
  virtual ~Connection();
  void Open(const char* fname);
  void Close();
  static std::unique_ptr<Connection> Create(const char* fname = nullptr);

private:
  int open_internal(const char* fname);
  int close_internal();
  Connection();
  Connection(const char* fname);
  
  sqlite3* db_;

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;
};

} // namespace sql

#endif // KVNSFER_SQLITE_CONNECTION_H_
