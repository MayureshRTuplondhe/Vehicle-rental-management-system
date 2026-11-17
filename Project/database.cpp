#include "database.hpp"

#include "rental_error.hpp"

#include <fstream>
#include <sstream>

// Reuses a single SQLite connection and enables relationship checks per
// connection.
Database &Database::instance() {
  static Database database;
  return database;
}

Database::Database() : db_(NULL) {
  const int result = sqlite3_open("rental.db", &db_);
  if (result != SQLITE_OK) {
    const std::string message =
        db_ ? sqlite3_errmsg(db_) : "could not allocate SQLite connection";
    if (db_)
      sqlite3_close(db_);
    db_ = NULL;
    throw RentalError("Could not open rental.db: " + message);
  }
  execute("PRAGMA foreign_keys = ON;");
  sqlite3_busy_timeout(db_, 5000);

  // Apply the checked-in schema, then add fields for older database files.
  std::ifstream schemaFile("schema.sql");
  if (!schemaFile)
    throw RentalError(
        "Could not open schema.sql; run the server from Project/.");
  std::ostringstream schemaText;
  schemaText << schemaFile.rdbuf();
  execute(schemaText.str());
  migrateSchema();
  seedIfEmpty();
}

Database::~Database() {
  if (db_)
    sqlite3_close(db_);
}

sqlite3 *Database::handle() const { return db_; }

void Database::execute(const std::string &sql) { execute(sql.c_str()); }

void Database::execute(const char *sql) {
  char *error = NULL;
  const int result = sqlite3_exec(db_, sql, NULL, NULL, &error);
  if (result != SQLITE_OK) {
    const std::string message = error ? error : sqlite3_errmsg(db_);
    sqlite3_free(error);
    throw RentalError("SQLite error: " + message);
  }
}

void Database::seedIfEmpty() {
  const auto readSeedFile = [](const char *path) {
    std::ifstream file(path);
    if (!file)
      throw RentalError(std::string("Could not open ") + path +
                        "; run the server from Project/.");
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
  };

  // The seed scripts ignore rows that already exist, so they can safely fill
  // in missing demo accounts or vehicles even when other real rows are present.
  execute("BEGIN TRANSACTION;");
  try {
    execute(readSeedFile("seed.sql"));
    execute(readSeedFile("seed_vehicles.sql"));
    execute("COMMIT;");
  } catch (...) {
    execute("ROLLBACK;");
    throw;
  }
}

void Database::migrateSchema() {
  const char *migrations[] = {
      "ALTER TABLE users ADD COLUMN password_text TEXT",
      "ALTER TABLE vehicles ADD COLUMN category TEXT NOT NULL DEFAULT 'Sedan'",
      "ALTER TABLE vehicles ADD COLUMN fuel TEXT NOT NULL DEFAULT 'Petrol'",
      "ALTER TABLE vehicles ADD COLUMN seats INTEGER NOT NULL DEFAULT 5",
      "ALTER TABLE bookings ADD COLUMN pickup_location TEXT NOT NULL DEFAULT "
      "''",
      "ALTER TABLE vehicles ADD COLUMN is_active INTEGER NOT NULL DEFAULT 1 "
      "CHECK (is_active IN (0, 1))",
      "ALTER TABLE payments ADD COLUMN status TEXT NOT NULL DEFAULT 'PAID' "
      "CHECK (status IN ('PENDING', 'PAID', 'FAILED'))",
      "ALTER TABLE payments ADD COLUMN transaction_ref TEXT",
      "ALTER TABLE payments ADD COLUMN method TEXT NOT NULL DEFAULT 'DEMO'"};
  const char *tables[] = {"users",    "vehicles", "vehicles",
                          "vehicles", "bookings", "vehicles",
                          "payments", "payments", "payments"};
  const char *columns[] = {"password_text", "category",        "fuel",
                           "seats",         "pickup_location", "is_active",
                           "status",        "transaction_ref", "method"};
  for (int i = 0; i < 9; ++i) {
    const std::string pragma =
        std::string("PRAGMA table_info(") + tables[i] + ")";
    sqlite3_stmt *statement = NULL;
    if (sqlite3_prepare_v2(db_, pragma.c_str(), -1, &statement, NULL) !=
        SQLITE_OK)
      throw RentalError("Could not inspect database schema.");
    bool found = false;
    while (sqlite3_step(statement) == SQLITE_ROW) {
      const unsigned char *name = sqlite3_column_text(statement, 1);
      if (name &&
          std::string(reinterpret_cast<const char *>(name)) == columns[i])
        found = true;
    }
    sqlite3_finalize(statement);
    if (!found)
      execute(migrations[i]);
  }
}

// Prepared statements keep input data separate from SQL commands.
Statement::Statement(sqlite3 *db, const std::string &sql) : stmt_(NULL) {
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt_, NULL) != SQLITE_OK)
    throw RentalError(std::string("Could not prepare SQL: ") +
                      sqlite3_errmsg(db));
}

Statement::~Statement() {
  if (stmt_)
    sqlite3_finalize(stmt_);
}

void Statement::bind(int index, int value) {
  check(sqlite3_bind_int(stmt_, index, value));
}

void Statement::bind(int index, double value) {
  check(sqlite3_bind_double(stmt_, index, value));
}

void Statement::bind(int index, const std::string &value) {
  check(sqlite3_bind_text(stmt_, index, value.c_str(), -1, SQLITE_TRANSIENT));
}

void Statement::bindNull(int index) { check(sqlite3_bind_null(stmt_, index)); }

bool Statement::step() {
  const int result = sqlite3_step(stmt_);
  if (result == SQLITE_ROW)
    return true;
  if (result == SQLITE_DONE)
    return false;
  throw RentalError(std::string("SQLite query failed: ") +
                    sqlite3_errmsg(sqlite3_db_handle(stmt_)));
}

int Statement::integer(int column) const {
  return sqlite3_column_int(stmt_, column);
}

double Statement::real(int column) const {
  return sqlite3_column_double(stmt_, column);
}

std::string Statement::text(int column) const {
  const unsigned char *value = sqlite3_column_text(stmt_, column);
  return value ? reinterpret_cast<const char *>(value) : "";
}

void Statement::check(int result) {
  if (result != SQLITE_OK)
    throw RentalError(std::string("SQLite bind failed: ") +
                      sqlite3_errmsg(sqlite3_db_handle(stmt_)));
}
