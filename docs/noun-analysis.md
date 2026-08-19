<!-- # Vehicle-rental-management-system
 TeamId - 34
 Team-mates
 1.Parth Mhaskar - 20251501184 <br>
 2.Mayuresh Tuplondhe - 20251501169 <br>
 3.Parth Mhaske - 20251502021 <br>
 4.Abhishek Patil - 20251502002 <br>
 5.N Sai Praneeth - 20251502019 <br> -->
 
## Vehicle Rental Management System

## 1. Purpose

This document identifies the important nouns in the Vehicle Rental Service System and determines which of them should become classes in the Object-Oriented Design.

The analysis is based on the project problem statement and the sequence diagrams for:

- Customer
- Renter
- Admin

The process used is:

**Candidate Nouns → Apply Filters → Surviving Nouns → Preliminary Classes**

---

## 1. Specification 1 — User Management

### Candidate Nouns

- User
- Admin
- Customer
- Renter
- Profile
- Login
- Credentials

### Candidate Verbs

- Login
- Logout
- View
- Update
- Search
- Check
- Rent
- Return
- Display
- Authenticate
- Manage
- Monitor

### Main Responsibilities

- **User:** Login, logout, authentication
- **Customer:** Rent vehicles, return vehicles, view rentals
- **Renter:** Manage vehicles, view rentals, view earnings
- **Admin:** Manage users, vehicles and rentals, monitor the system

---

# 2. Specification 2 — Vehicle Management

### Candidate Nouns

- Vehicle
- Car
- Bike
- Vehicle Details
- Vehicle Status
- Availability
- Price
- Daily Price
- Vehicle Inventory

### Candidate Verbs

- Add
- View
- Update
- Remove
- Search
- Check
- Manage
- Rent
- Return

### Main Responsibilities

- **Vehicle:** Maintain vehicle information, price, status, and availability
- **Car:** Represent a car available for rental
- **Bike:** Represent a bike available for rental
- **Admin/Renter:** Add, update, view, and remove vehicles
- **Customer:** Search vehicles and check availability

---

# 3. Specification 3 — Rental and Billing

### Candidate Nouns

- Rental
- Customer
- Renter
- Vehicle
- Bill
- Rental Details
- Rental Information
- Rental Duration
- Days / Number of Days
- Rental Amount
- Rental Cost
- Payment
- Revenue
- Earnings
- Status

### Candidate Verbs

- Create
- Rent
- Return
- Associate
- Track
- Calculate
- Generate
- Display
- Pay
- Update

### Main Responsibilities

- **Rental:** Create rental, associate customer and vehicle, track rental, and calculate rental amount
- **Bill:** Generate and display bill
- **Customer:** Rent and return vehicle
- **Vehicle:** Update availability after rental/return

---

# 4. Combined Raw Candidate List

After combining the nouns from all three specifications and removing only exact duplicate occurrences, the raw candidate list is:

| No. | Raw Candidate | Source |
|---:|---|---|
| 1 | User | User Management |
| 2 | Admin | User Management |
| 3 | Customer | User Management |
| 4 | Renter | User Management |
| 5 | Profile | User Management |
| 6 | Login | User Management |
| 7 | Credentials | User Management |
| 8 | Vehicle | Vehicle Management |
| 9 | Car | Vehicle Management |
| 10 | Bike | Vehicle Management |
| 11 | Vehicle Details | Vehicle Management |
| 12 | Vehicle Status | Vehicle Management |
| 13 | Availability | Vehicle Management |
| 14 | Price | Vehicle Management |
| 15 | Daily Price | Vehicle Management |
| 16 | Vehicle Inventory | Vehicle Management |
| 17 | Rental | Rental and Billing |
| 18 | Bill | Rental and Billing |
| 19 | Rental Details | Rental and Billing |
| 20 | Rental Information | Rental and Billing |
| 21 | Rental Duration | Rental and Billing |
| 22 | Days / Number of Days | Rental and Billing |
| 23 | Rental Amount | Rental and Billing |
| 24 | Rental Cost | Rental and Billing |
| 25 | Payment | Rental and Billing |
| 26 | Revenue | Rental and Billing |
| 27 | Earnings | Rental and Billing |
| 28 | Status | Rental and Billing |

---

# 5. Application of the Four Filters

The four filters are applied as follows:

1. **Duplicate / Redundancy Filter**  
   Removes candidates that represent the same or overlapping concept.

2. **Attribute Filter**  
   Removes properties, information, or derived values that should be attributes of another class.

3. **Operation / Verb Filter**  
   Removes actions that should become methods or responsibilities instead of classes.

4. **Implementation / System Filter**  
   Removes system-level or implementation concepts that are not domain classes.

---

# 6. Candidate Evaluation

| Candidate | Decision | Reason / Filter |
|---|---|---|
| User | **Survives** | Independent domain concept |
| Admin | **Survives** | Independent user role |
| Customer | **Survives** | Independent user role |
| Renter | **Survives** | Independent user role |
| Profile | Discarded | Attribute Filter — information belonging to User |
| Login | Discarded | Operation / Verb Filter — authentication operation |
| Credentials | Discarded | Attribute Filter — User information |
| Vehicle | **Survives** | Core domain entity |
| Car | **Survives** | Specialized type of Vehicle |
| Bike | **Survives** | Specialized type of Vehicle |
| Vehicle Details | Discarded | Attribute Filter — information belonging to Vehicle |
| Vehicle Status | Discarded | Attribute Filter — Vehicle property |
| Availability | Discarded | Attribute Filter — Vehicle state |
| Price | Discarded | Attribute Filter — Vehicle property |
| Daily Price | Discarded | Attribute Filter — Vehicle property |
| Vehicle Inventory | Discarded | Implementation / System Filter — collection/management concept |
| Rental | **Survives** | Independent rental transaction |
| Bill | **Survives** | Independent billing entity |
| Rental Details | Discarded | Duplicate / Attribute Filter — information belonging to Rental |
| Rental Information | Discarded | Duplicate / Attribute Filter — information belonging to Rental |
| Rental Duration | Discarded | Attribute Filter — Rental property |
| Days / Number of Days | Discarded | Attribute Filter — Rental property |
| Rental Amount | Discarded | Attribute Filter — calculated Rental/Bill value |
| Rental Cost | Discarded | Attribute Filter — calculated Rental/Bill value |
| Payment | Discarded | Implementation / System Filter — no independent payment class required in current specification |
| Revenue | Discarded | Attribute Filter — derived financial information |
| Earnings | Discarded | Attribute Filter — derived financial information |
| Status | Discarded | Attribute Filter — Rental property |

---

# 7. Surviving and Discarded Classes

## Surviving Classes

| No. | Class |
|---:|---|
| 1 | User |
| 2 | Admin |
| 3 | Customer |
| 4 | Renter |
| 5 | Vehicle |
| 6 | Car |
| 7 | Bike |
| 8 | Rental |
| 9 | Bill |

## Discarded Candidates

| Candidate | Eliminating Filter |
|---|---|
| Profile | Attribute Filter |
| Login | Operation / Verb Filter |
| Credentials | Attribute Filter |
| Vehicle Details | Attribute Filter |
| Vehicle Status | Attribute Filter |
| Availability | Attribute Filter |
| Price | Attribute Filter |
| Daily Price | Attribute Filter |
| Vehicle Inventory | Implementation / System Filter |
| Rental Details | Duplicate / Attribute Filter |
| Rental Information | Duplicate / Attribute Filter |
| Rental Duration | Attribute Filter |
| Days / Number of Days | Attribute Filter |
| Rental Amount | Attribute Filter |
| Rental Cost | Attribute Filter |
| Payment | Implementation / System Filter |
| Revenue | Attribute Filter |
| Earnings | Attribute Filter |
| Status | Attribute Filter |

---

# 7. Final Class Set

The final candidate classes to be used for CRC analysis and the domain class diagram are:

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