# RentRide Project Guide

This file explains the technology stack, architecture, database, and OOP
concepts that are present in the current RentRide vehicle-rental project. It is
an additional guide; it does not replace `README.md` or change the original
`PROJECT.c++` file.

## Project overview

RentRide is a local vehicle-rental web application. A browser UI is served by a
small C++ server. The server exposes HTTP endpoints, applies rental rules, and
reads or updates persistent records in SQLite.

```text
Browser: rentride (2).html + app.js
                    | HTTP on localhost
                    v
C++ backend: HTTP server + booking service + repositories
                    | SQLite C API
                    v
              rental.db
```

## Technology stack

| Area               | Technology used                       | Purpose                                                                                                            |
| ------------------ | ------------------------------------- | ------------------------------------------------------------------------------------------------------------------ |
| Backend language   | C++11                                 | Server, models, persistence access, and rental operations                                                          |
| HTTP server        | Windows Winsock (`ws2_32`)            | Serves the UI and handles local HTTP/API requests; this is a small project server, not a third-party web framework |
| Database           | SQLite 3.50.4                         | Stores users, vehicles, bookings, and payment-table data in `rental.db`                                            |
| SQLite integration | SQLite C API and bundled amalgamation | The C++ database wrapper opens the database and executes prepared statements                                       |
| Frontend           | HTML5, CSS, and vanilla JavaScript    | Page structure, styling, browser interactions, and API requests; no frontend framework is used                     |
| Build tools        | MinGW GCC/G++ or CMake                | Builds the C++11 backend; the manual build links SQLite and Windows Winsock                                        |

The backend does not depend on an external JSON library or password-hashing
package. It returns JSON from its API code and accepts URL-encoded form data.
Passwords in this coursework demo are stored as plain text. Use only test
accounts on localhost; this is not a production authentication design.

## C++ class count

There are **17 declared C++ classes** in the current backend headers, including
`Runnable` and `PaymentWorker`, plus **2 structs** used internally by the HTTP
server. The browser UI in `app.js` does
not define a second set of JavaScript model classes; it uses the C++ API as its
source of data.

### The 17 C++ classes

| Class               | Role                                                                              |
| ------------------- | --------------------------------------------------------------------------------- |
| `Vehicle`           | Base vehicle model with private data, getters, availability, and rent calculation |
| `Car`               | Vehicle type derived from `Vehicle`                                               |
| `Bike`              | Vehicle type derived from `Vehicle`                                               |
| `Truck`             | Vehicle type derived from `Vehicle`                                               |
| `VehicleFactory`    | Creates a `Car`, `Bike`, or `Truck` model from stored type data                   |
| `User`              | Base user model with identity and a virtual role method                           |
| `Customer`          | User type derived from `User`                                                     |
| `Renter`            | User type derived from `User`                                                     |
| `IRepository`       | Common repository base class                                                      |
| `UserRepository`    | Database operations for user records                                              |
| `VehicleRepository` | Database operations for vehicle records                                           |
| `Database`          | Owns the SQLite connection and initializes schema/seed data                       |
| `Statement`         | Owns a prepared SQLite statement and its bound parameters                         |
| `BookingService`    | Applies booking, return, and rental business rules                                |
| `RentalError`       | Project exception type derived from `std::runtime_error`                          |
| `Runnable`          | Abstract interface with the worker's `run()` operation                            |
| `PaymentWorker`     | Owns a synchronized queue and background `std::thread` for demo payments          |

The two internal structs are `HttpRequest` and `Session`. Structs are not
included in the 17-class count.

## OOP concepts actually used

### Encapsulation and access control

Model and database classes keep implementation data private and expose
operations through public methods. For example, `Vehicle` stores its ID,
owner, registration, daily rate, and status in private members, then provides
getters such as `id()`, `rate()`, and `isAvailable()`.

### Abstraction

Callers use the public methods of classes such as `Database`, `Statement`,
`VehicleRepository`, and `BookingService` without needing to manage SQLite
handles and SQL details themselves. `IRepository` provides a common
repository base for the concrete repositories.

### Inheritance

The backend uses these inheritance relationships:

- `Car`, `Bike`, and `Truck` derive from `Vehicle`.
- `Customer` and `Renter` derive from `User`.
- `PaymentWorker` implements the `Runnable` interface.
- `UserRepository` and `VehicleRepository` derive from `IRepository`.
- `RentalError` derives from the standard-library `std::runtime_error`.

### Polymorphism

`Vehicle` declares virtual `type()` and `calculateRent()` methods. `User`
declares a virtual `role()` method, which `Customer` and `Renter` override.
The `VehicleFactory` returns `std::unique_ptr<Vehicle>` objects that may own a
derived vehicle, allowing the program to work through the base type while
preserving the derived type behavior. The base classes have virtual destructors
so derived objects can be destroyed safely through base pointers.

### Compile-time polymorphism (function overloading)

The database layer overloads functions with different parameter types:

- `Database::execute(const char *)` and `Database::execute(const std::string &)`
- `Statement::bind(...)` for `int`, `double`, and `std::string`

The compiler selects the appropriate function from the argument types.

### Constructors, destructors, and non-copyable resources

The classes initialize their state in constructors and release owned SQLite
resources in destructors. Copy construction and copy assignment are deleted
for `Database` and `Statement`; this prevents accidental copying of a connection
or prepared statement that must have a single clear owner.

### RAII and smart pointers

`Database` owns the SQLite connection, and `Statement` finalizes its prepared
statement when its object leaves scope. This is RAII (Resource Acquisition Is
Initialization). `std::unique_ptr<Vehicle>` owns factory-created vehicles and
makes their lifetime automatic. Repository results use `std::vector`, but the
vector is only a temporary query result; SQLite remains the persistent source
of truth.

### Static members and methods

- `Database::instance()` is a static method that provides the single shared
  database connection (Singleton pattern).
- `VehicleFactory::create()` is a static factory method.
- The database singleton is held in a function-local static object.

### Exception handling

`RentalError` represents project errors and derives from `std::runtime_error`.
Database, repository, and service operations throw errors when operations fail;
the HTTP server catches exceptions and sends an error response instead of
allowing them to escape the request handler.

## Threading and asynchronous demo payments

`PaymentWorker` implements a Runnable-style interface and owns one C++
`std::thread`. Calling `start()` creates the thread, which executes `run()`.
Payment requests are stored in a `std::queue<int>` protected by `std::mutex`.
A Windows auto-reset event wakes the worker when a payment is queued, while a
`CRITICAL_SECTION` protects the queue and stop flag. It uses `Sleep(1200)` to
simulate gateway delay, then updates the payment row in SQLite. The worker opens
its own SQLite connection so it does not share the server's connection across
threads. On Ctrl+C, the server signals the worker to finish queued work and
calls `WaitForSingleObject` to join it before shutdown.

Java's `Thread` inheritance, `Runnable` interface, `sleep()`, and `yield()` are
not C++ APIs. This Windows/MinGW build demonstrates their equivalents with
`_beginthreadex`, the project's `Runnable` interface, `Sleep`, and
`WaitForSingleObject`. `CRITICAL_SECTION` and an auto-reset event synchronize
the queue. `SwitchToThread` (the yield-like operation) is not used because the
worker waits on an event rather than busy-waiting.
Payments are simulated and do not contact a real payment provider or charge
money.

## Design patterns and layering

- **Singleton:** `Database::instance()` provides the backend's shared SQLite
  connection.
- **Factory:** `VehicleFactory::create()` constructs the correct derived
  vehicle model for a stored vehicle type.
- **Repository:** `UserRepository` and `VehicleRepository` isolate database
  queries from the rest of the application.
- **Service layer:** `BookingService` holds booking and return rules rather
  than putting all business logic in the HTTP handler.

## SQLite data

`schema.sql` creates the main tables and constraints:

- `users` — admin, renter, and customer accounts
- `vehicles` — vehicle details and owning renter
- `bookings` — customer/vehicle rental periods and totals
- `payments` — payment records and simulated processing status

Foreign keys connect owners, customers, vehicles, and bookings. Constraints
validate roles, vehicle status, positive rates, and booking dates. The backend
also seeds missing demo accounts and vehicles at startup using `seed.sql` and
`seed_vehicles.sql`; the seed data provides 50 renters, 50 customers, and one
vehicle for each demo renter. Demo account passwords are `demo1234`.

## OOP topics not implemented in the current backend

The separate `OOP_CONCEPTS.md` file discusses additional teaching examples.
Those examples should not be mistaken for implemented backend features. The
current backend does **not** define custom examples of operator overloading,
friend functions/classes, nested classes, virtual inheritance, object slicing,
`final`, user-defined templates, or custom type-conversion operators. The
standard-library templates `std::vector` and `std::unique_ptr` are used, but the
project does not define its own generic template class or function.

## Important files

- `backend.cpp` — program entry point.
- `http_server.cpp/.hpp` — localhost HTTP server and API endpoints.
- `app.js` — browser behavior and calls to the C++ API.
- `rentride (2).html` — final RentRide page served by the backend.
- `models.hpp/.cpp` — user and vehicle classes, inheritance, and factory.
- `repositories.hpp/.cpp` — SQLite-backed user and vehicle repositories.
- `booking_service.hpp/.cpp` — booking and return rules.
- `payment_worker.hpp/.cpp` — synchronized background payment processing.
- `database.hpp/.cpp` — SQLite connection, schema setup, seeding, and statements.
- `rental_error.hpp` — project exception type.
- `schema.sql`, `seed.sql`, `seed_vehicles.sql` — database schema and demo data.
- `rental.db` — local SQLite database file.
- `PROJECT.c++` — original terminal-based reference program; kept separate from
  the web backend.
