#include "connection.h"
#include "sqlite3.h"
#include <assert.h>

namespace sql {

Connection::Connection()
  : db_(nullptr) {}

Connection::Connection(const char* fname)
  : Connection() {
  Open(fname);
}

Connection::~Connection() {
  close_internal();
}

void Connection::Open(const char* fname) {
  int rc = open_internal(fname);
  if (rc) {

  }
}

void Connection::Close() {
  close_internal();
}

int Connection::open_internal(const char* fname) {
  assert(!db_);
  if (db_) return;
  int rc = sqlite3_open(fname, &db_);
  return rc;
}

int Connection::close_internal() {
  assert(db_);
  if (!db_) return;
  int rc = sqlite3_close(db_);
  return rc;
}

// static
std::unique_ptr<Connection> Connection::Create(const char* fname) {
  if (fname) {
    return std::unique_ptr<Connection>(new Connection(fname));
  }
  else {
    return std::unique_ptr<Connection>(new Connection);
  }
}

} // namespace sql