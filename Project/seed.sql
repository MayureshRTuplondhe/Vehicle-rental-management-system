-- Local test accounts. All seeded accounts use the password: demo1234
WITH RECURSIVE numbers(n) AS (
    VALUES (1)
    UNION ALL
    SELECT n + 1 FROM numbers WHERE n < 50
)
INSERT INTO users(name, email, phone, password_text, role, license_no)
SELECT
    printf('Demo Renter %02d', n),
    printf('renter%02d@example.test', n),
    printf('555-010-%04d', n),
    'demo1234',
    'RENTER',
    NULL
FROM numbers WHERE 1
ON CONFLICT(email) DO NOTHING;

WITH RECURSIVE numbers(n) AS (
    VALUES (1)
    UNION ALL
    SELECT n + 1 FROM numbers WHERE n < 50
)
INSERT INTO users(name, email, phone, password_text, role, license_no)
SELECT
    printf('Demo Customer %02d', n),
    printf('customer%02d@example.test', n),
    printf('555-020-%04d', n),
    'demo1234',
    'CUSTOMER',
    printf('DEMO-LIC-%04d', n)
FROM numbers WHERE 1
ON CONFLICT(email) DO NOTHING;
