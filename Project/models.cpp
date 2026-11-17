#include "models.hpp"

#include "rental_error.hpp"

Vehicle::Vehicle(int id, int ownerId, const std::string &type,
                 const std::string &name, const std::string &registration,
                 double rate, const std::string &status)
    : id_(id), ownerId_(ownerId), type_(type), name_(name),
      registration_(registration), rate_(rate), status_(status) {}

Vehicle::~Vehicle() {}
int Vehicle::id() const { return id_; }
int Vehicle::ownerId() const { return ownerId_; }
const std::string &Vehicle::name() const { return name_; }
const std::string &Vehicle::registration() const { return registration_; }
double Vehicle::rate() const { return rate_; }
const std::string &Vehicle::status() const { return status_; }
bool Vehicle::isAvailable() const { return status_ == "AVAILABLE"; }
std::string Vehicle::type() const { return type_; }

double Vehicle::calculateRent(int days) const {
  if (days <= 0)
    throw RentalError("Rental days must be greater than zero.");
  return rate_ * days;
}

Car::Car(int id, int ownerId, const std::string &name,
         const std::string &registration, double rate,
         const std::string &status)
    : Vehicle(id, ownerId, "CAR", name, registration, rate, status) {}
std::string Car::type() const { return "CAR"; }

Bike::Bike(int id, int ownerId, const std::string &name,
           const std::string &registration, double rate,
           const std::string &status)
    : Vehicle(id, ownerId, "BIKE", name, registration, rate, status) {}
std::string Bike::type() const { return "BIKE"; }

Truck::Truck(int id, int ownerId, const std::string &name,
             const std::string &registration, double rate,
             const std::string &status)
    : Vehicle(id, ownerId, "TRUCK", name, registration, rate, status) {}
std::string Truck::type() const { return "TRUCK"; }

std::unique_ptr<Vehicle>
VehicleFactory::create(int id, int ownerId, const std::string &type,
                       const std::string &name, const std::string &registration,
                       double rate, const std::string &status) {
  if (type == "CAR")
    return std::unique_ptr<Vehicle>(
        new Car(id, ownerId, name, registration, rate, status));
  if (type == "BIKE")
    return std::unique_ptr<Vehicle>(
        new Bike(id, ownerId, name, registration, rate, status));
  if (type == "TRUCK")
    return std::unique_ptr<Vehicle>(
        new Truck(id, ownerId, name, registration, rate, status));
  throw RentalError("Unsupported vehicle type.");
}

User::User(int id, const std::string &name, const std::string &role)
    : id_(id), name_(name), role_(role) {}
User::~User() {}
int User::id() const { return id_; }
const std::string &User::name() const { return name_; }
std::string User::role() const { return role_; }

Customer::Customer(int id, const std::string &name)
    : User(id, name, "CUSTOMER") {}
std::string Customer::role() const { return "CUSTOMER"; }

Renter::Renter(int id, const std::string &name) : User(id, name, "RENTER") {}
std::string Renter::role() const { return "RENTER"; }
