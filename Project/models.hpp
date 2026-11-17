#pragma once

#include <memory>
#include <string>

// Base vehicle type; derived classes override type() to demonstrate
// polymorphism.
class Vehicle {
    private:
    int id_;
    int ownerId_;
    std::string type_;
    std::string name_;
    std::string registration_;
    double rate_;
    std::string status_;
public:
  Vehicle(int id, int ownerId, const std::string &type, const std::string &name,
          const std::string &registration, double rate,
          const std::string &status);
  virtual ~Vehicle();

  int id() const;
  int ownerId() const;
  const std::string &name() const;
  const std::string &registration() const;
  double rate() const;
  const std::string &status() const;
  bool isAvailable() const;
  virtual std::string type() const;
  virtual double calculateRent(int days) const;
};

class Car : public Vehicle {
public:
  Car(int id, int ownerId, const std::string &name,
      const std::string &registration, double rate, const std::string &status);
  std::string type() const;
};

class Bike : public Vehicle {
public:
  Bike(int id, int ownerId, const std::string &name,
       const std::string &registration, double rate, const std::string &status);
  std::string type() const;
};

class Truck : public Vehicle {
public:
  Truck(int id, int ownerId, const std::string &name,
        const std::string &registration, double rate,
        const std::string &status);
  std::string type() const;
};

// Converts a stored vehicle type into the matching derived OOP model.
class VehicleFactory {
public:
  static std::unique_ptr<Vehicle>
  create(int id, int ownerId, const std::string &type, const std::string &name,
         const std::string &registration, double rate,
         const std::string &status);
};

class User {
public:
  User(int id, const std::string &name, const std::string &role);
  virtual ~User();
  int id() const;
  const std::string &name() const;
  virtual std::string role() const;

private:
  int id_;
  std::string name_;
  std::string role_;
};

class Customer : public User {
public:
  Customer(int id, const std::string &name);
  std::string role() const;
};

class Renter : public User {
public:
  Renter(int id, const std::string &name);
  std::string role() const;
};
