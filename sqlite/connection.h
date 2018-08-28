#ifndef KVNSFER_SQLITE_CONNECTION_H_
#define KVNSFER_SQLITE_CONNECTION_H_

#include "apr.h"

struct sqlite3;

namespace sql {

class Connection {
public:
  Connection();
  Connection(const char* fname);
  virtual ~Connection();

  void Open(const char* fname);
  void Close();

private:
  sqlite3* db_;

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;
};

} // namespace sql

#endif // KVNSFER_SQLITE_CONNECTION_H_
