#include "booking_service.hpp"

#include "database.hpp"
#include "rental_error.hpp"
#include "repositories.hpp"

#include <iostream>
#include <sqlite3.h>

int BookingService::book(int customerId, int vehicleId,
                         const std::string &start, const std::string &end,
                         const std::string &pickupLocation) {
  if (!UserRepository().exists(customerId, "CUSTOMER"))
    throw RentalError("Customer account was not found.");

  Statement vehicle(
      Database::instance().handle(),
      "SELECT rate_per_day,status,is_active FROM vehicles WHERE id=?");
  vehicle.bind(1, vehicleId);
  if (!vehicle.step())
    throw RentalError("Vehicle not found.");
  const double rate = vehicle.real(0);
  if (vehicle.integer(2) == 0)
    throw RentalError("Vehicle has been removed from the active fleet.");
  if (vehicle.text(1) == "MAINTENANCE")
    throw RentalError("Vehicle is under maintenance.");

  // SQLite counts both the pickup and return dates as rental days.
  // The schema trigger rejects overlapping active bookings.
  Statement insert(
      Database::instance().handle(),
      "INSERT INTO "
      "bookings(customer_id,vehicle_id,start_date,end_date,total_amount,pickup_"
      "location) "
      "SELECT ?,?,?,?,? * (julianday(?) - julianday(?) + 1),? "
      "WHERE date(?) IS NOT NULL AND date(?) IS NOT NULL AND date(?) >= "
      "date(?)");
  insert.bind(1, customerId);
  insert.bind(2, vehicleId);
  insert.bind(3, start);
  insert.bind(4, end);
  insert.bind(5, rate);
  insert.bind(6, end);
  insert.bind(7, start);
  insert.bind(8, pickupLocation);
  insert.bind(9, start);
  insert.bind(10, end);
  insert.bind(11, end);
  insert.bind(12, start);
  insert.step();
  if (sqlite3_changes(Database::instance().handle()) == 0)
    throw RentalError("Dates must be valid ISO dates (YYYY-MM-DD), with end on "
                      "or after start.");
  return static_cast<int>(
      sqlite3_last_insert_rowid(Database::instance().handle()));
}

void BookingService::returnVehicle(int bookingId, int customerId) {
  Statement update(
      Database::instance().handle(),
      "UPDATE bookings SET status='RETURNED', returned_on=date('now') "
      "WHERE id=? AND customer_id=? AND status='ACTIVE'");
  update.bind(1, bookingId);
  update.bind(2, customerId);
  update.step();
  if (sqlite3_changes(Database::instance().handle()) == 0)
    throw RentalError("Active booking for this customer was not found.");
}

void BookingService::showForCustomer(int customerId) {
  Statement query(
      Database::instance().handle(),
      "SELECT b.id,v.name,b.start_date,b.end_date,b.total_amount,b.status "
      "FROM bookings b JOIN vehicles v ON v.id=b.vehicle_id "
      "WHERE b.customer_id=? ORDER BY b.id DESC");
  query.bind(1, customerId);
  std::cout << "ID | Vehicle | Start | End | Total | Status\n";
  while (query.step()) {
    std::cout << query.integer(0) << " | " << query.text(1) << " | "
              << query.text(2) << " | " << query.text(3) << " | $"
              << query.real(4) << " | " << query.text(5) << '\n';
  }
}
