<!-- # Vehicle-rental-management-system
 TeamId - 34
 Team-mates
 1.Parth Mhaskar - 20251501184 <br>
 2.Mayuresh Tuplondhe - 20251501169 <br>
 3.Parth Mhaske - 20251502021 <br>
 4.Abhishek Patil - 20251502002 <br>
 5.N Sai Praneeth - 20251502019 <br> -->
 
# CRC Cards — Vehicle Rental Management System

## 1. Purpose

CRC (Class–Responsibility–Collaborator) cards identify the main classes in the Vehicle Rental Management System, define what each class is responsible for, and identify the other classes with which it interacts.

The CRC analysis uses `User` as the parent class for `Customer`, `Renter`, and `Admin`. Similarly, `Vehicle` is an abstract parent class for `Car` and `Bike`.

### Main Classes

- User
- Customer
- Renter
- Admin
- Vehicle
- Car
- Bike
- Rental
- Bill

---

# 2. CRC Cards

## CRC-01: User

| Class | User |
|---|---|
| **Type** | Parent Class |

### Responsibilities

- Store common information shared by all users.
- Support user authentication.
- Allow users to log out.
- Maintain common profile information.
- Provide common functionality to user subclasses.
- Serve as the parent class for `Customer`, `Renter`, and `Admin`.

### Collaborators

- Customer
- Renter
- Admin

### Collaboration

`User` provides the common abstraction for all types of users in the system. `Customer`, `Renter`, and `Admin` inherit the common attributes and operations from `User`.

---

## CRC-02: Customer

| Class | Customer |
|---|---|
| **Type** | User Subclass |

### Responsibilities

- Store the customer's driving license information.
- Search for vehicles.
- View available vehicles.
- Rent an available vehicle.
- Return a rented vehicle.
- View rental information.
- View bills associated with rentals.

### Collaborators

- User
- Vehicle
- Rental
- Bill

### Collaboration

`Customer` inherits common functionality from `User` and interacts with `Vehicle` to search for and rent vehicles. `Rental` records the customer's rental transaction, while `Bill` provides the billing information.

---

## CRC-03: Renter

| Class | Renter |
|---|---|
| **Type** | User Subclass |

### Responsibilities

- Store the renter's rental experience.
- Add vehicles to the rental system.
- View vehicles managed by the renter.
- Update vehicle information.
- View rentals associated with the renter's vehicles.

### Collaborators

- User
- Vehicle
- Rental

### Collaboration

`Renter` inherits common functionality from `User` and manages `Vehicle` objects. The renter can view the vehicles they manage and the rentals associated with those vehicles.

---

## CRC-04: Admin

| Class | Admin |
|---|---|
| **Type** | User Subclass |

### Responsibilities

- View all users.
- View all vehicles.
- View all rentals.
- View overall system statistics.
- Monitor major entities of the rental system.

### Collaborators

- User
- Vehicle
- Rental
- Bill

### Collaboration

`Admin` inherits common functionality from `User` and monitors the major entities of the system. The administrator can view users, vehicles, rentals, and billing information as part of system monitoring and statistics.

---

## CRC-05: Vehicle

| Class | Vehicle |
|---|---|
| **Type** | Abstract Parent Class |

### Responsibilities

- Represent a vehicle available for rental.
- Store common vehicle information.
- Maintain vehicle registration and identification information.
- Store the vehicle brand and model.
- Maintain the daily rental price.
- Maintain the availability status.
- Check vehicle availability.
- Calculate rental charges.
- Update vehicle status.
- Serve as the parent class for `Car` and `Bike`.

### Collaborators

- Customer
- Renter
- Rental
- Car
- Bike

### Collaboration

`Vehicle` is the abstract parent class for all rentable vehicles. `Customer` rents vehicles, `Renter` manages vehicles, and `Rental` records the vehicle involved in a rental. `Car` and `Bike` inherit common functionality from `Vehicle`.

---

## CRC-06: Car

| Class | Car |
|---|---|
| **Type** | Vehicle Subclass |

### Responsibilities

- Represent a car available for rental.
- Store the seating capacity of the car.
- Store the fuel type of the car.
- Calculate the rental amount for a car.
- Inherit common vehicle information and behavior from `Vehicle`.

### Collaborators

- Vehicle
- Customer
- Renter
- Rental

### Collaboration

`Car` inherits the common properties and operations of `Vehicle` and adds car-specific information such as seating capacity and fuel type. It overrides `calculateRent()` to provide car-specific rental calculation.

---

## CRC-07: Bike

| Class | Bike |
|---|---|
| **Type** | Vehicle Subclass |

### Responsibilities

- Represent a bike available for rental.
- Store the type of bike.
- Calculate the rental amount for a bike.
- Inherit common vehicle information and behavior from `Vehicle`.

### Collaborators

- Vehicle
- Customer
- Renter
- Rental

### Collaboration

`Bike` inherits the common properties and operations of `Vehicle` and adds bike-specific information through `bikeType`. It overrides `calculateRent()` to provide bike-specific rental calculation.

---

## CRC-08: Rental

| Class | Rental |
|---|---|
| **Type** | Domain Entity |

### Responsibilities

- Represent a vehicle rental transaction.
- Associate a customer with a vehicle.
- Store the pickup date.
- Store the return date.
- Maintain the rental status.
- Store the total rental amount.
- Start a rental.
- Record the return of a vehicle.
- Calculate the total rental amount.
- Provide rental information for billing.

### Collaborators

- Customer
- Vehicle
- Renter
- Bill

### Collaboration

`Rental` is the central transaction connecting a `Customer` with a `Vehicle`. It records the rental dates, status, and total amount. `Bill` uses the rental information to represent the charges generated from the rental.

---

## CRC-09: Bill

| Class | Bill |
|---|---|
| **Type** | Domain Entity |

### Responsibilities

- Represent the bill generated for a rental.
- Store bill identification information.
- Store the bill date.
- Store the payable amount.
- Maintain the payment method.
- Maintain the payment status.
- Generate the bill.
- Provide billing information to the customer.

### Collaborators

- Rental
- Customer
- Admin

### Collaboration

`Bill` is associated with a `Rental` and represents the charges generated from that rental. `Customer` can view the bill, while `Admin` can monitor billing information as part of system management.

---

## 3. Inheritance Structure

The CRC analysis supports the following inheritance relationships:

```text
                         User
                           ▲
              ┌────────────┼────────────┐
              │            │            │
          Customer       Renter        Admin


                       Vehicle
                          ▲
                    ┌─────┴─────┐
                    │           │
                   Car         Bike
```
---

## 4. Core Collaboration Structure

```text
Customer ────── rents ──────> Vehicle
    │                           │
    │                           │
    └──── creates ─────────> Rental
                                │
                                │ generates
                                ▼
                              Bill

Renter ────── manages ──────> Vehicle

Admin ─────── monitors ──────> User
Admin ─────── monitors ──────> Vehicle
Admin ─────── monitors ──────> Rental
Admin ─────── monitors ──────> Bill
```
---

## 5. CRC Summary

| Class        | Main Responsibilities                                                      | Main Collaborators                  |
| ------------ | -------------------------------------------------------------------------- | ----------------------------------- |
| **User**     | Common user information, authentication, logout, and profile               | Customer, Renter, Admin             |
| **Customer** | Search, view, rent, return vehicles, and view rentals and bills            | User, Vehicle, Rental, Bill         |
| **Renter**   | Add, view, and update vehicles; view rentals                               | User, Vehicle, Rental               |
| **Admin**    | View users, vehicles, rentals, and system statistics                       | User, Vehicle, Rental, Bill         |
| **Vehicle**  | Store vehicle information, availability, price, status, and calculate rent | Customer, Renter, Rental, Car, Bike |
| **Car**      | Store car-specific information and calculate car rental                    | Vehicle, Customer, Renter, Rental   |
| **Bike**     | Store bike-specific information and calculate bike rental                  | Vehicle, Customer, Renter, Rental   |
| **Rental**   | Manage rental transaction, dates, status, return, and total amount         | Customer, Vehicle, Renter, Bill     |
| **Bill**     | Generate and provide billing information                                   | Rental, Customer, Admin             |

---

## 6. Conclusion

The CRC analysis identifies the responsibilities and collaborations of the **nine classes** in the Vehicle Rental Management System.

The main design decisions are:

1. `User` is the parent class for `Customer`, `Renter`, and `Admin`.
2. `Vehicle` is an abstract parent class for `Car` and `Bike`.
3. `Customer` searches for, rents, and returns vehicles.
4. `Renter` adds, views, and updates vehicles and views their rentals.
5. `Admin` monitors users, vehicles, rentals, and system statistics.
6. `Vehicle` stores common vehicle information and provides common vehicle operations.
7. `Car` and `Bike` provide specialized vehicle information and override `calculateRent()`.
8. `Rental` represents the central transaction between a `Customer` and a `Vehicle`.
9. `Bill` represents the billing information generated for a rental.

### Final Class Structure

```text
User
├── Customer
├── Renter
└── Admin

Vehicle
├── Car
└── Bike

Rental
Bill
```