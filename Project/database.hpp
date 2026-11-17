#pragma once

#include <sqlite3.h>

#include <string>

// Owns the application's one SQLite connection (Singleton + RAII).
class Database {
public:
  static Database &instance();
  sqlite3 *handle() const;

  Database(const Database &) = delete;
  Database &operator=(const Database &) = delete;
  ~Database();

private:
  Database();
  void execute(const char *sql);
  void execute(const std::string &sql);
  void migrateSchema();
  void seedIfEmpty();
  sqlite3 *db_;
};

// Owns one prepared SQL statement; binds values and finalizes it automatically.
class Statement {
public:
  Statement(sqlite3 *db, const std::string &sql);
  ~Statement();

  Statement(const Statement &) = delete;
  Statement &operator=(const Statement &) = delete;

  void bind(int index, int value);
  void bind(int index, double value);
  void bind(int index, const std::string &value);
  void bindNull(int index);
  bool step();
  int integer(int column) const;
  double real(int column) const;
  std::string text(int column) const;

private:
  void check(int result);
  sqlite3_stmt *stmt_;
};
