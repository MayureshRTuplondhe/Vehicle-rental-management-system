#pragma once

#include <stdexcept>
#include <string>

// Common error type used by the database, repositories, and services.
class RentalError : public std::runtime_error {
public:
  explicit RentalError(const std::string &message)
      : std::runtime_error(message) {}
};
