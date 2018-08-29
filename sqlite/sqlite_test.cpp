#include "connection.h"

int main(int argc, char *argv[]) {
  std::unique_ptr<sql::Connection> conn = sql::Connection::Create("test.db");
}