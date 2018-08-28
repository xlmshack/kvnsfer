#include "connection.h"
#include "sqlite3.h"
#include <assert.h>

namespace sql {

Connection::Connection()
  : db_(nullptr) {}

Connection::Connection(const char* fname)
  : Connection() {
  int rc = sqlite3_open(fname, &db_);

}

Connection::~Connection() {

}

void Connection::Open(const char* fname) {

}

} // namespace sql