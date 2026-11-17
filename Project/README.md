# RentRide local demo

This project keeps the supplied `PROJECT.c++` unchanged and uses the OOP models,
SQLite repositories, and booking service in this folder as the web backend for
`rentride (2).html`.

## Files

- `backend.cpp` — small program entry point.
- `vrms_backend_fixed.exe` — latest backend build, including vehicle archiving and threaded demo payments.
- `http_server.cpp/.hpp` — local Windows HTTP server and simple API.
- `app.js` — browser UI; loads and changes data through the API.
- `database.hpp/.cpp` — SQLite connection, schema initialization, and prepared
  statements.
- `models.hpp/.cpp` — OOP users/vehicles, inheritance, and vehicle factory.
- `repositories.hpp/.cpp` — database-backed user/vehicle persistence.
- `booking_service.cpp/.hpp` — booking, date overlap, and return rules.
- `payment_worker.cpp/.hpp` — synchronized background worker for simulated demo payments.
- `schema.sql` — users, vehicles, bookings, payments, indexes, and constraints.
- `seed.sql` — inserts any missing demo renters and customers.
- `seed_vehicles.sql` — inserts missing demo vehicles for the demo renters.
- `rental.db` — local SQLite data file; missing demo users and vehicles are added on backend startup.
- `sqlite-amalgamation-3500400/` — SQLite 3.50.4 C source/header used to build
  the SQLite library required by the C++ backend.

`PROJECT.c++` remains unchanged as the original terminal reference. The web
backend uses the SQLite repositories instead of in-memory vectors for storage;
`std::vector` is used only to hold rows returned by a query while rendering a
response, not as the source of truth.

## Build and run

Build on Windows with a C++11 MinGW compiler. SQLite 3.50.4 is bundled in the
folder above; Windows Winsock is linked with `ws2_32`. From this folder, run:

```bat
gcc -O2 -c sqlite-amalgamation-3500400\sqlite3.c -o sqlite3.o
g++ -std=c++11 -Wall -Wextra -pedantic -Isqlite-amalgamation-3500400 backend.cpp http_server.cpp database.cpp models.cpp repositories.cpp booking_service.cpp payment_worker.cpp sqlite3.o -lws2_32 -o vrms_backend_fixed.exe
```

Create the first admin locally (once):

```bat
vrms_backend_fixed.exe --create-admin
```

Then start the server:

```bat
vrms_backend_fixed.exe
```

Open `http://127.0.0.1:8080` in the browser. Do not open the HTML with `file://`;
the page and API need to be served from the same local server. Run the program
from this folder so it can read `schema.sql`, `rental.db`, `app.js`, and
`rentride (2).html`.

## Demo accounts and security

Seeded accounts use `demo1234` as the password. For example, log in with
`renter01@example.test` or `customer01@example.test`; customer accounts are
numbered through `customer50@example.test`. The 50 demo renters each own one
demo vehicle. User and vehicle seeding are checked independently: users are
added when their demo email is missing, and vehicles when their demo registration is missing. This fills missing seed data without duplicating records.

To keep this coursework version simple, passwords are stored as plain text in
the local SQLite file and login sessions only exist in memory until the server
stops. Demo payments run asynchronously on a C++ worker thread and do not charge
real money. Use test-only credentials. This server binds to localhost and is not
suitable for public networks or production use.
