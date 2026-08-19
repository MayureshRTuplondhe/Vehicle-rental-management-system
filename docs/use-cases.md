<!-- # Vehicle-rental-management-system
 TeamId - 34
 Team-mates
 1.Parth Mhaskar - 20251501184 <br>
 2.Mayuresh Tuplondhe - 20251501169 <br>
 3.Parth Mhaske - 20251502021 <br>
 4.Abhishek Patil - 20251502002 <br>
 5.N Sai Praneeth - 20251502019 <br> -->
 
# Vehicle Rental System — Use Case Specification

## 1. System Overview

The **Vehicle Rental System** is a software system that manages vehicle rentals between customers and renters while providing administrative monitoring and management functionality.

The system supports three primary actors:

- **Customer** — searches and rents available vehicles, manages rentals, views bills, and returns vehicles.
- **Renter** — owns/lists vehicles for rental and monitors their rentals and earnings.
- **Admin** — monitors users, vehicles, rentals, and overall system statistics.

The main system component is the `VehicleRentalSystem`, which coordinates operations with internal repositories such as:

- `UserRepository`
- `VehicleRepository`
- `RentalRepository`
- `BillRepository`

These repositories are internal implementation components and are therefore not considered external actors.

---

# 2. Actors

## 2.1 Customer

A customer is a user who rents vehicles from the system.

### Customer capabilities

- Login
- View profile
- View available vehicles
- Search for vehicles
- Rent a vehicle
- View personal rentals
- View rental bill
- Return a vehicle
- Logout

---

## 2.2 Renter

A renter is a user who provides vehicles for rental through the system.

### Renter capabilities

- Login
- View profile
- Add a vehicle
- View their vehicles
- Update vehicle details
- Remove a vehicle
- View their rentals
- View earnings
- Logout

---

## 2.3 Admin

An administrator manages and monitors the overall system.

### Admin capabilities

- Login
- View profile
- View all users
- View all vehicles
- View all rentals
- View system statistics
- Logout

---

# 3. Use Case List

## Customer Use Cases

| ID | Use Case | Actor |
|---|---|---|
| C-01 | Login | Customer |
| C-02 | View Profile | Customer |
| C-03 | View Available Vehicles | Customer |
| C-04 | Search Vehicle | Customer |
| C-05 | Rent Vehicle | Customer |
| C-06 | View My Rentals | Customer |
| C-07 | View Bill | Customer |
| C-08 | Return Vehicle | Customer |
| C-09 | Logout | Customer |

## Renter Use Cases

| ID | Use Case | Actor |
|---|---|---|
| R-01 | Login | Renter |
| R-02 | View Profile | Renter |
| R-03 | Add Vehicle | Renter |
| R-04 | View My Vehicles | Renter |
| R-05 | Update Vehicle | Renter |
| R-06 | Remove Vehicle | Renter |
| R-07 | View My Rentals | Renter |
| R-08 | View Earnings | Renter |
| R-09 | Logout | Renter |

## Admin Use Cases

| ID | Use Case | Actor |
|---|---|---|
| A-01 | Login | Admin |
| A-02 | View Profile | Admin |
| A-03 | View All Users | Admin |
| A-04 | View All Vehicles | Admin |
| A-05 | View All Rentals | Admin |
| A-06 | View System Statistics | Admin |
| A-07 | Logout | Admin |

---

# 4. Customer Use Cases

## C-01: Login

**Actor:** Customer

**Description:** Allows a customer to authenticate into the Vehicle Rental System using their login ID and password.

### Preconditions

- Customer has valid login credentials.
- System is available.

### Main Flow

1. Customer enters login ID and password.
2. System validates the credentials.
3. System authenticates the customer.
4. System returns a successful login response.
5. Customer is granted access to the system.

### Alternate Flow

- If the credentials are invalid, login is unsuccessful.

### Postcondition

- Customer is successfully authenticated and can access customer functionality.

---

## C-02: View Profile

**Actor:** Customer

**Description:** Allows the customer to view their profile information.

### Main Flow

1. Customer selects **View Profile**.
2. Customer sends `viewProfile()` request.
3. System retrieves the customer's profile.
4. System returns the customer profile.
5. Profile information is displayed.

### Postcondition

- Customer profile is displayed.

---

## C-03: View Available Vehicles

**Actor:** Customer

**Description:** Allows a customer to view vehicles currently available for rental.

### Main Flow

1. Customer requests available vehicles.
2. System calls `getAvailableVehicles()`.
3. Vehicle repository retrieves available vehicles.
4. Available vehicle list is returned to the system.
5. System displays the available vehicles to the customer.

### Postcondition

- Customer can see vehicles that are currently available.

---

## C-04: Search Vehicle

**Actor:** Customer

**Description:** Allows a customer to search for vehicles using specified criteria.

### Main Flow

1. Customer enters search criteria.
2. System receives `searchVehicle(criteria)`.
3. System sends the criteria to the vehicle repository.
4. Repository searches for matching vehicles.
5. Matching vehicle details are returned.
6. System displays the vehicle details.

### Postcondition

- Matching vehicles are displayed to the customer.

---

## C-05: Rent Vehicle

**Actor:** Customer

**Description:** Allows a customer to rent an available vehicle for a specified number of days.

### Preconditions

- Customer is logged in.
- Vehicle exists.
- Vehicle is available.

### Main Flow

1. Customer selects a vehicle.
2. Customer specifies the number of rental days.
3. System calls `rentVehicle(vehicleId, days)`.
4. System checks vehicle availability using `checkAvailability(vehicleId)`.
5. If the vehicle is available, the system creates a rental using `createRental(customerId, vehicleId, days)`.
6. Rental is successfully created.
7. System calculates the rental amount using `calculateAmount(rentalId)`.
8. System generates a bill using `generateBill(rentalId)`.
9. System displays rental and bill information to the customer.

### Alternate Flow

- If the selected vehicle is unavailable, the rental cannot be created.

### Postcondition

- A rental is created for the customer.
- A bill is generated.
- The vehicle is associated with the active rental.

---

## C-06: View My Rentals

**Actor:** Customer

**Description:** Allows the customer to view their rental history/details.

### Main Flow

1. Customer selects **View My Rentals**.
2. System calls `viewMyRentals()`.
3. System requests rentals using `getRentalsByCustomer(customerId)`.
4. Rental repository returns the customer's rental list.
5. System displays the rental list.

### Postcondition

- Customer's rentals are displayed.

---

## C-07: View Bill

**Actor:** Customer

**Description:** Allows the customer to view the bill associated with a rental.

### Main Flow

1. Customer selects a rental.
2. Customer requests the associated bill.
3. System calls `getBill(rentalId)`.
4. Bill repository retrieves the bill.
5. Bill details are returned.
6. System displays the bill.

### Postcondition

- The bill associated with the rental is displayed.

---

## C-08: Return Vehicle

**Actor:** Customer

**Description:** Allows a customer to return a rented vehicle and complete the rental.

### Preconditions

- Customer has an active rental.

### Main Flow

1. Customer selects the rental to return.
2. Customer calls `returnVehicle(rentalId)`.
3. System requests the rental to be completed.
4. System calls `completeRental(rentalId)`.
5. Rental is marked as completed.
6. System updates the vehicle status to `AVAILABLE`.
7. System confirms successful return.
8. Final rental/bill information is displayed.

### Postcondition

- Rental is completed.
- Vehicle status is changed to `AVAILABLE`.

---

## C-09: Logout

**Actor:** Customer

**Description:** Allows the customer to end their current session.

### Main Flow

1. Customer selects logout.
2. System calls `logout()`.
3. System terminates the customer session.
4. Logout confirmation is displayed.

### Postcondition

- Customer is logged out.

---

# 5. Renter Use Cases

## R-01: Login

**Actor:** Renter

**Description:** Allows a renter to authenticate into the system.

### Main Flow

1. Renter enters login ID and password.
2. System validates the credentials.
3. System authenticates the renter.
4. Successful login is returned.

---

## R-02: View Profile

**Actor:** Renter

**Description:** Allows the renter to view their profile details.

### Main Flow

1. Renter selects **View Profile**.
2. System calls `viewProfile()`.
3. System retrieves the renter's profile.
4. Profile details are returned and displayed.

---

## R-03: Add Vehicle

**Actor:** Renter

**Description:** Allows a renter to add a vehicle to the system for rental.

### Main Flow

1. Renter selects **Add Vehicle**.
2. Renter enters vehicle details.
3. System calls `addVehicle(details)`.
4. System requests the vehicle inventory to create the vehicle.
5. Vehicle is created successfully.
6. System confirms that the vehicle was added.

### Postcondition

- New vehicle is added to the renter's vehicle inventory.

---

## R-04: View My Vehicles

**Actor:** Renter

**Description:** Allows the renter to view the vehicles they have added to the system.

### Main Flow

1. Renter selects **View My Vehicles**.
2. System calls `viewMyVehicles()`.
3. System requests vehicles using `getVehiclesByRenter(renterId)`.
4. Vehicle inventory returns the renter's vehicle list.
5. System displays the vehicle list.

---

## R-05: Update Vehicle

**Actor:** Renter

**Description:** Allows a renter to update the details of a vehicle they own.

### Preconditions

- Vehicle exists.
- Vehicle belongs to the logged-in renter.
- Vehicle is not currently rented.

### Main Flow

1. Renter selects a vehicle.
2. Renter provides new vehicle details.
3. System calls `updateVehicle(vehicleId, newDetails)`.
4. System attempts to update the vehicle.
5. Vehicle details are updated.
6. System returns an update-success response.

### Alternate Flows

**Vehicle not found**

- System throws `VehicleNotFoundException`.

**Vehicle is currently rented**

- System throws `VehicleRentedException`.

### Postcondition

- Vehicle details are updated successfully.

---

## R-06: Remove Vehicle

**Actor:** Renter

**Description:** Allows a renter to remove one of their vehicles from the system.

### Preconditions

- Vehicle exists.
- Vehicle belongs to the logged-in renter.
- Vehicle is not currently rented.

### Main Flow

1. Renter selects a vehicle to remove.
2. System calls `removeVehicle(vehicleId)`.
3. System checks whether the vehicle can be removed.
4. Vehicle is removed from the system.
5. System confirms successful removal.

### Alternate Flows

**Vehicle not found**

- System throws `VehicleNotFoundException`.

**Vehicle is currently rented**

- System throws `VehicleRentedException`.

**Vehicle belongs to another renter**

- System throws `UnauthorizedException`.

### Postcondition

- Vehicle is removed from the renter's inventory.

---

## R-07: View My Rentals

**Actor:** Renter

**Description:** Allows a renter to view rentals associated with their vehicles.

### Main Flow

1. Renter selects **View My Rentals**.
2. System calls `viewMyRentals()`.
3. System requests rentals using `getRentalsByRenter(renterId)`.
4. Rental repository returns the rental list.
5. System displays the rental list.

---

## R-08: View Earnings

**Actor:** Renter

**Description:** Allows a renter to view the total earnings generated from their vehicles.

### Main Flow

1. Renter selects **View Earnings**.
2. System calls `viewEarnings()`.
3. System requests earnings using `getTotalEarnings(renterId)`.
4. Rental component calculates/retrieves the renter's total earnings.
5. Earnings are returned.
6. System displays total earnings.

### Postcondition

- Renter's total earnings are displayed.

---

## R-09: Logout

**Actor:** Renter

**Description:** Allows the renter to terminate their session.

### Main Flow

1. Renter selects logout.
2. System calls `logout()`.
3. System terminates the renter session.
4. Logout confirmation is displayed.

---

# 6. Admin Use Cases

## A-01: Login

**Actor:** Admin

**Description:** Allows an administrator to authenticate into the system.

### Main Flow

1. Admin enters login ID and password.
2. System validates the credentials.
3. System authenticates the administrator.
4. Successful login is returned.

---

## A-02: View Profile

**Actor:** Admin

**Description:** Allows the administrator to view their profile.

### Main Flow

1. Admin selects **View Profile**.
2. System calls `viewProfile()`.
3. System retrieves the administrator's profile.
4. Profile information is displayed.

---

## A-03: View All Users

**Actor:** Admin

**Description:** Allows the administrator to view all registered users.

### Main Flow

1. Admin selects **View All Users**.
2. System calls `getAllUsers()`.
3. System requests users using `fetchUsers()`.
4. User repository returns the user list.
5. System displays all users.

### Users may include

- Customers
- Renters
- Administrators

---

## A-04: View All Vehicles

**Actor:** Admin

**Description:** Allows the administrator to view all vehicles registered in the system.

### Main Flow

1. Admin selects **View All Vehicles**.
2. System calls `getAllVehicles()`.
3. System requests vehicles using `fetchVehicles()`.
4. Vehicle repository returns the vehicle list.
5. System displays all vehicles.

---

## A-05: View All Rentals

**Actor:** Admin

**Description:** Allows the administrator to view all rentals in the system.

### Main Flow

1. Admin selects **View All Rentals**.
2. System calls `getAllRentals()`.
3. System requests rentals using `fetchRentals()`.
4. Rental repository returns the rental list.
5. System displays all rentals.

---

## A-06: View System Statistics

**Actor:** Admin

**Description:** Allows the administrator to view overall statistics about the vehicle rental system.

### Main Flow

1. Admin selects **View System Statistics**.
2. System calls `getSystemStatistics()`.
3. System retrieves user counts.
4. System retrieves vehicle counts.
5. System retrieves rental counts.
6. System retrieves total revenue.
7. System compiles the statistics.
8. System displays the statistics to the administrator.

### Statistics displayed

- Total users
- Total customers/renters
- Total vehicles
- Available vehicles
- Rented vehicles
- Total rentals
- Active rentals
- Completed rentals
- Total revenue

---

## A-07: Logout

**Actor:** Admin

**Description:** Allows the administrator to terminate their session.

### Main Flow

1. Admin selects logout.
2. System calls `logout()`.
3. System terminates the administrator session.
4. Logout confirmation is displayed.

---

# 7. Use Case Relationships

The following relationships can be represented in the UML use-case diagram.

## Customer

### Rent Vehicle

`Rent Vehicle` includes:

- Check Vehicle Availability
- Create Rental
- Calculate Rental Amount
- Generate Bill

```text
Rent Vehicle
    |
    +-- <<include>> Check Vehicle Availability
    |
    +-- <<include>> Create Rental
    |
    +-- <<include>> Calculate Rental Amount
    |
    +-- <<include>> Generate Bill
```

### Return Vehicle

`Return Vehicle` includes:

- Complete Rental
- Update Vehicle Status

```text
Return Vehicle
    |
    +-- <<include>> Complete Rental
    |
    +-- <<include>> Update Vehicle Status
```

---

# 8. Important Business Rules

1. A customer can rent a vehicle only when the vehicle is available.
2. A rental is associated with a specific customer, vehicle, and number of rental days.
3. A bill is generated for a successfully created rental.
4. A customer can view their own rentals.
5. A customer can view the bill associated with their rental.
6. Completing a rental changes the vehicle status to `AVAILABLE`.
7. A renter can add vehicles to the system.
8. A renter can update their vehicle details only when the vehicle is not currently rented.
9. A renter cannot remove a vehicle that is currently rented.
10. A renter cannot modify or remove a vehicle belonging to another renter.
11. Attempting to access a non-existent vehicle results in `VehicleNotFoundException`.
12. Attempting to modify a rented vehicle results in `VehicleRentedException`.
13. Attempting to modify/remove another renter's vehicle results in `UnauthorizedException`.
14. Admin can view system-wide users, vehicles, and rentals.
15. Admin can view overall system statistics including revenue.

---

# 9. System Boundary

The **Vehicle Rental System** contains all the functionality described above.

### External Actors

```text
+-----------------------------------------------+
|             Vehicle Rental System             |
|                                               |
|  Customer                                      |
|    - Login                                     |
|    - View Profile                              |
|    - Search Vehicle                            |
|    - View Available Vehicles                   |
|    - Rent Vehicle                              |
|    - View My Rentals                           |
|    - View Bill                                 |
|    - Return Vehicle                            |
|    - Logout                                    |
|                                               |
|  Renter                                        |
|    - Login                                     |
|    - View Profile                              |
|    - Add Vehicle                               |
|    - View My Vehicles                          |
|    - Update Vehicle                             |
|    - Remove Vehicle                             |
|    - View My Rentals                           |
|    - View Earnings                             |
|    - Logout                                    |
|                                               |
|  Admin                                         |
|    - Login                                     |
|    - View Profile                              |
|    - View All Users                             |
|    - View All Vehicles                          |
|    - View All Rentals                           |
|    - View System Statistics                    |
|    - Logout                                    |
|                                               |
+-----------------------------------------------+
```

---

# 10. Summary

The Vehicle Rental System provides three different perspectives:

### Customer

The customer is primarily concerned with **finding, renting, managing, and returning vehicles**.

### Renter

The renter is primarily concerned with **providing vehicles, managing their vehicles, monitoring rentals, and tracking earnings**.

### Admin

The administrator is primarily concerned with **monitoring users, vehicles, rentals, and system-wide statistics**.

The sequence diagrams show that `VehicleRentalSystem` acts as the main coordinator between actors and internal repositories. The repositories handle data access for users, vehicles, rentals, and bills.
