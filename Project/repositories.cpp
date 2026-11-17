#include "repositories.hpp"

#include "database.hpp"
#include "rental_error.hpp"

#include <sqlite3.h>

int UserRepository::add(const std::string &name, const std::string &email,
                        const std::string &phone, const std::string &role,
                        const std::string &license) {
  // Placeholders ensure user input is bound as data rather than SQL text.
  Statement statement(
      Database::instance().handle(),
      "INSERT INTO users(name,email,phone,role,license_no) VALUES(?,?,?,?,?)");
  statement.bind(1, name);
  statement.bind(2, email);
  statement.bind(3, phone);
  statement.bind(4, role);
  if (license.empty())
    statement.bindNull(5);
  else
    statement.bind(5, license);
  statement.step();
  return static_cast<int>(
      sqlite3_last_insert_rowid(Database::instance().handle()));
}

bool UserRepository::exists(int id, const std::string &role) {
  Statement statement(Database::instance().handle(),
                      "SELECT 1 FROM users WHERE id=? AND role=?");
  statement.bind(1, id);
  statement.bind(2, role);
  return statement.step();
}

int VehicleRepository::add(int ownerId, const std::string &type,
                           const std::string &name,
                           const std::string &registration, double rate) {
  if (!UserRepository().exists(ownerId, "RENTER"))
    throw RentalError("Owner must be a registered renter.");
  if (type != "CAR" && type != "BIKE" && type != "TRUCK")
    throw RentalError("Vehicle type must be CAR, BIKE, or TRUCK.");
  if (name.empty() || registration.empty() || rate <= 0)
    throw RentalError(
        "Name, registration, and positive daily rate are required.");

  Statement statement(
      Database::instance().handle(),
      "INSERT INTO vehicles(owner_id,type,name,reg_number,rate_per_day) "
      "VALUES(?,?,?,?,?)");
  statement.bind(1, ownerId);
  statement.bind(2, type);
  statement.bind(3, name);
  statement.bind(4, registration);
  statement.bind(5, rate);
  statement.step();
  return static_cast<int>(
      sqlite3_last_insert_rowid(Database::instance().handle()));
}

std::vector<std::unique_ptr<Vehicle>>
VehicleRepository::all(bool availableOnly) {
  const std::string sql =
      availableOnly
          ? "SELECT "
            "v.id,v.owner_id,v.type,v.name,v.reg_number,v.rate_per_day,v."
            "status "
            "FROM vehicles v WHERE v.is_active=1 AND v.status='AVAILABLE' "
            "AND NOT EXISTS "
            "(SELECT 1 FROM bookings b WHERE b.vehicle_id=v.id "
            "AND b.status='ACTIVE' AND date('now') BETWEEN date(b.start_date) "
            "AND date(b.end_date)) ORDER BY v.id"
          : "SELECT id,owner_id,type,name,reg_number,rate_per_day,status FROM "
            "vehicles WHERE is_active=1 ORDER BY id";

  Statement statement(Database::instance().handle(), sql);
  std::vector<std::unique_ptr<Vehicle>> vehicles;
  while (statement.step()) {
    vehicles.push_back(VehicleFactory::create(
        statement.integer(0), statement.integer(1), statement.text(2),
        statement.text(3), statement.text(4), statement.real(5),
        statement.text(6)));
  }
  return vehicles;
}

std::unique_ptr<Vehicle> VehicleRepository::find(int id) {
  Statement statement(
      Database::instance().handle(),
      "SELECT id,owner_id,type,name,reg_number,rate_per_day,status FROM "
      "vehicles WHERE id=? AND is_active=1");
  statement.bind(1, id);
  if (!statement.step())
    throw RentalError("Vehicle not found.");
  return VehicleFactory::create(statement.integer(0), statement.integer(1),
                                statement.text(2), statement.text(3),
                                statement.text(4), statement.real(5),
                                statement.text(6));
}
