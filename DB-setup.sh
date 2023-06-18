#!/bin/bash

# Update the system and install PostgreSQL
sudo apt update
sudo apt install -y postgresql

# Set up PostgreSQL database and user
sudo -u postgres psql -c "CREATE DATABASE customer_db;"
sudo -u postgres psql -c "CREATE USER customer_user WITH ENCRYPTED PASSWORD 'password';"
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE customer_db TO customer_user;"

# Connect to the customer_db database
sudo -u postgres psql -d customer_db << EOF

-- Create the customer_profile table
CREATE TABLE customer_profile (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100),
    email VARCHAR(100),
    phone VARCHAR(20)
);

-- Create the bags table
CREATE TABLE bags (
    id SERIAL PRIMARY KEY,
    weight DECIMAL(5,2),
    origin VARCHAR(3),
    destination VARCHAR(3),
    profile_id INTEGER REFERENCES customer_profile(id)
);

-- Add sample data
INSERT INTO customer_profile (name, email, phone)
VALUES ('John Doe', 'johndoe@example.com', '1234567890');

INSERT INTO bags (weight, origin, destination, profile_id)
VALUES (20.5, 'JFK', 'LAX', 1);

INSERT INTO bags (weight, origin, destination, profile_id)
VALUES (10.2, 'LHR', 'CDG', 1);

EOF

# Restart PostgreSQL service
sudo service postgresql restart

echo "Database setup completed."
