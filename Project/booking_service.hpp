#pragma once

#include <string>

// Business rules for booking, returning, and showing a customer's rentals.
class BookingService {
public:
  int book(int customerId, int vehicleId, const std::string &start,
           const std::string &end, const std::string &pickupLocation = "");
  void returnVehicle(int bookingId, int customerId);
  void showForCustomer(int customerId);
};
