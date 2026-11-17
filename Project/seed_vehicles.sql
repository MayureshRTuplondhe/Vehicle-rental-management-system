-- Add one example vehicle owned by each of the 50 demo renters.
-- INSERT OR IGNORE makes this safe to run whenever the backend starts.
WITH RECURSIVE numbers(n) AS (
    VALUES (1)
    UNION ALL
    SELECT n + 1 FROM numbers WHERE n < 50
)
INSERT OR IGNORE INTO vehicles(owner_id, type, category, name, reg_number, fuel, seats, rate_per_day)
SELECT
    users.id,
    CASE
        WHEN numbers.n <= 35 THEN 'CAR'
        WHEN numbers.n <= 45 THEN 'BIKE'
        ELSE 'TRUCK'
    END,
    CASE
        WHEN numbers.n <= 10 THEN 'Sedan'
        WHEN numbers.n <= 20 THEN 'SUV'
        WHEN numbers.n <= 30 THEN 'Hatchback'
        WHEN numbers.n <= 35 THEN 'MPV'
        WHEN numbers.n <= 45 THEN 'Bike'
        ELSE 'Truck'
    END,
    CASE
        WHEN numbers.n <= 10 THEN printf('Demo Sedan %02d', numbers.n)
        WHEN numbers.n <= 20 THEN printf('Demo SUV %02d', numbers.n - 10)
        WHEN numbers.n <= 30 THEN printf('Demo Hatchback %02d', numbers.n - 20)
        WHEN numbers.n <= 35 THEN printf('Demo MPV %02d', numbers.n - 30)
        WHEN numbers.n <= 45 THEN printf('Demo Bike %02d', numbers.n - 35)
        ELSE printf('Demo Truck %02d', numbers.n - 45)
    END,
    printf('DEMO-VEH-%04d', numbers.n),
    CASE WHEN numbers.n % 3 = 0 THEN 'Diesel' ELSE 'Petrol' END,
    CASE
        WHEN numbers.n > 45 THEN 2
        WHEN numbers.n > 35 THEN 2
        WHEN numbers.n <= 10 THEN 5
        WHEN numbers.n <= 20 THEN 7
        ELSE 5
    END,
    CASE
        WHEN numbers.n > 45 THEN 120.0 + numbers.n
        WHEN numbers.n > 35 THEN 35.0 + numbers.n
        ELSE 55.0 + numbers.n * 2
    END
FROM numbers
JOIN users ON users.email = printf('renter%02d@example.test', numbers.n);
