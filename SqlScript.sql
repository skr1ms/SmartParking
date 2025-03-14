CREATE TABLE vehicle_types (
    id SERIAL PRIMARY KEY,
    type_name VARCHAR(50) NOT NULL,
    price_multiplier DECIMAL(3,2) NOT NULL DEFAULT 1.0
);

CREATE TABLE vehicles (
    id SERIAL PRIMARY KEY,
    license_plate VARCHAR(20) NOT NULL UNIQUE,
    brand VARCHAR(50) NOT NULL,
    model VARCHAR(50) NOT NULL,
    owner_name VARCHAR(100) NOT NULL,
    vehicle_type_id INTEGER REFERENCES vehicle_types(id),
    registration_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE tariffs (
    id SERIAL PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    description TEXT,
    price DECIMAL(10,2) NOT NULL
);

CREATE TABLE parking_spots (
    id SERIAL PRIMARY KEY,
    spot_number INTEGER NOT NULL UNIQUE,
    is_occupied BOOLEAN DEFAULT FALSE,
    vehicle_id INTEGER REFERENCES vehicles(id)
);

CREATE TABLE user_accounts (
    id SERIAL PRIMARY KEY,
    owner_name VARCHAR(100) NOT NULL UNIQUE,
    balance DECIMAL(10,2) NOT NULL DEFAULT 0.0
);

CREATE TABLE parking_sessions (
    id SERIAL PRIMARY KEY,
    vehicle_id INTEGER REFERENCES vehicles(id),
    parking_spot_id INTEGER REFERENCES parking_spots(id),
    tariff_id INTEGER REFERENCES tariffs(id),
    entry_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    exit_time TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    payment_amount DECIMAL(10,2)
);

CREATE TABLE transaction_history (
    id SERIAL PRIMARY KEY,
    user_account_id INTEGER REFERENCES user_accounts(id),
    amount DECIMAL(10,2) NOT NULL,
    transaction_type VARCHAR(20) NOT NULL, -- 'deposit', 'payment'
    transaction_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    description TEXT
);

INSERT INTO vehicle_types (type_name, price_multiplier) VALUES
('Легковой автомобиль', 1.0),
('Грузовой автомобиль', 1.5);

INSERT INTO tariffs (name, description, price) VALUES
('Базовый', 'Стандартный тариф для парковки', 300.00),
('Улучшенный', 'Расширенный тариф с дополнительными услугами', 600.00),
('Максимальный', 'Полный набор услуг и приоритетное обслуживание', 1200.00);

DO $$
BEGIN
    FOR i IN 1..10 LOOP
        INSERT INTO parking_spots (spot_number, is_occupied) VALUES (i, FALSE);
    END LOOP;
END $$;