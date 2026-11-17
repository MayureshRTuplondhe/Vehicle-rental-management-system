PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    email TEXT NOT NULL UNIQUE,
    phone TEXT NOT NULL,
    password_text TEXT,
    role TEXT NOT NULL CHECK (role IN ('ADMIN', 'RENTER', 'CUSTOMER')),
    license_no TEXT,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CHECK (role != 'CUSTOMER' OR license_no IS NOT NULL)
);

CREATE TABLE IF NOT EXISTS vehicles (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    owner_id INTEGER NOT NULL REFERENCES users(id) ON DELETE RESTRICT,
    type TEXT NOT NULL CHECK (type IN ('CAR', 'BIKE', 'TRUCK')),
    category TEXT NOT NULL DEFAULT 'Sedan' CHECK (category IN ('MPV','SUV','Sedan','Hatchback','Pickup Truck','Bike','Truck')),
    name TEXT NOT NULL,
    reg_number TEXT NOT NULL UNIQUE,
    fuel TEXT NOT NULL DEFAULT 'Petrol',
    seats INTEGER NOT NULL DEFAULT 5 CHECK (seats > 0),
    rate_per_day REAL NOT NULL CHECK (rate_per_day > 0),
    status TEXT NOT NULL DEFAULT 'AVAILABLE'
        CHECK (status IN ('AVAILABLE', 'RENTED', 'MAINTENANCE')),
    is_active INTEGER NOT NULL DEFAULT 1 CHECK (is_active IN (0, 1))
);

CREATE TABLE IF NOT EXISTS bookings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    customer_id INTEGER NOT NULL REFERENCES users(id) ON DELETE RESTRICT,
    vehicle_id INTEGER NOT NULL REFERENCES vehicles(id) ON DELETE RESTRICT,
    start_date TEXT NOT NULL,
    end_date TEXT NOT NULL,
    returned_on TEXT,
    pickup_location TEXT NOT NULL DEFAULT '',
    total_amount REAL NOT NULL CHECK (total_amount >= 0),
    late_fee REAL NOT NULL DEFAULT 0 CHECK (late_fee >= 0),
    status TEXT NOT NULL DEFAULT 'ACTIVE'
        CHECK (status IN ('ACTIVE', 'RETURNED', 'CANCELLED')),
    CHECK (date(start_date) IS NOT NULL),
    CHECK (date(end_date) IS NOT NULL),
    CHECK (date(end_date) >= date(start_date)),
    CHECK (returned_on IS NULL OR date(returned_on) IS NOT NULL)
);

CREATE TABLE IF NOT EXISTS payments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    booking_id INTEGER NOT NULL REFERENCES bookings(id) ON DELETE RESTRICT,
    amount REAL NOT NULL CHECK (amount > 0),
    method TEXT NOT NULL DEFAULT 'DEMO',
    status TEXT NOT NULL DEFAULT 'PAID'
        CHECK (status IN ('PENDING', 'PAID', 'FAILED')),
    transaction_ref TEXT,
    paid_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_vehicles_status ON vehicles(status);
CREATE INDEX IF NOT EXISTS idx_vehicles_owner ON vehicles(owner_id);
CREATE INDEX IF NOT EXISTS idx_bookings_vehicle_dates
    ON bookings(vehicle_id, start_date, end_date);
CREATE INDEX IF NOT EXISTS idx_bookings_customer ON bookings(customer_id);

CREATE TRIGGER IF NOT EXISTS prevent_overlapping_active_bookings
BEFORE INSERT ON bookings
WHEN NEW.status = 'ACTIVE'
BEGIN
    SELECT CASE WHEN EXISTS (
        SELECT 1 FROM bookings
        WHERE vehicle_id = NEW.vehicle_id
          AND status = 'ACTIVE'
          AND date(NEW.start_date) <= date(end_date)
          AND date(NEW.end_date) >= date(start_date)
    ) THEN RAISE(ABORT, 'vehicle already has an overlapping active booking') END;
END;
