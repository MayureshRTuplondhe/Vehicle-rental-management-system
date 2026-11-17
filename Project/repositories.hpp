#pragma once

#include "models.hpp"

#include <string>
#include <vector>

// Repository interface keeps persistence details out of services and models.
class IRepository {
public:
  virtual ~IRepository() {}
};

class UserRepository : public IRepository {
public:
  int add(const std::string &name, const std::string &email,
          const std::string &phone, const std::string &role,
          const std::string &license);
  bool exists(int id, const std::string &role);
};

class VehicleRepository : public IRepository {
public:
  int add(int ownerId, const std::string &type, const std::string &name,
          const std::string &registration, double rate);
  std::vector<std::unique_ptr<Vehicle>> all(bool availableOnly);
  std::unique_ptr<Vehicle> find(int id);
};
