-- CREATE TABLE passenger (id INT PRIMARY KEY NOT NULL,name TEXT NOT NULL,contact char(10) NOT NULL); 
-- INSERT INTO passenger(id,name,contact) VALUES (1,'NAZARBEK','5488568555');
-- DELETE FROM passenger WHERE id = 1;
-- ALTER TABLE passenger ADD COLUMN email TEXT;
-- DROP TABLE passenger;
-- CREATE TABLE passenger (id INT PRIMARY KEY NOT NULL,name TEXT NOT NULL,contact char(10) NOT NULL,email TEXT NOT NULL);
-- CREATE TABLE bus (bus_id INTEGER PRIMARY KEY,bus_number TEXT,capacity INTEGER);
-- INSERT INTO bus(bus_id,bus_number,capacity) VALUES(1,'AC1232',35);
-- SELECT * FROM bus;
-- DELETE FROM bus WHERE bus_id = 1;
-- CREATE TABLE Route (route_id INTEGER PRIMARY KEY,source TEXT,destination TEXT);
-- CREATE TABLE Schedule (
--     schedule_id INTEGER PRIMARY KEY,
--     bus_id INTEGER,
--     route_id INTEGER,
--     departure_time TEXT,
--     FOREIGN KEY (bus_id) REFERENCES Bus(bus_id),
--     FOREIGN KEY (route_id) REFERENCES Route(route_id)
-- );
-- CREATE TABLE Ticket (
--     ticket_id INTEGER PRIMARY KEY,
--     passenger_id INTEGER,
--     bus_id INTEGER,
--     seat_number INTEGER,
--     journey_date TEXT,
--     status TEXT,
--     FOREIGN KEY (passenger_id) REFERENCES Passenger(passenger_id),
--     FOREIGN KEY (bus_id) REFERENCES Bus(bus_id)
-- );
-- CREATE TABLE Payment (
--     payment_id INTEGER PRIMARY KEY,
--     passenger_id INTEGER,
--     amount REAL,
--     payment_date TEXT,
--     payment_method TEXT,
--     FOREIGN KEY (passenger_id) REFERENCES Passenger(passenger_id)
-- );

-- INSERT INTO Bus (bus_id, bus_number, capacity) VALUES (1, 'BUS001', 50);
-- INSERT INTO Bus (bus_id, bus_number, capacity) VALUES (2, 'BUS002', 40);
-- INSERT INTO Bus (bus_id, bus_number, capacity) VALUES (3, 'BUS003', 60);
-- INSERT INTO Route (route_id, source, destination) VALUES (1, 'Girne', 'Lefkosa');
-- INSERT INTO Route (route_id, source, destination) VALUES (2, 'Lefkosa', 'Famagusta');
-- INSERT INTO Route (route_id, source, destination) VALUES (3, 'Girne', 'Famagusta');

-- INSERT INTO Schedule (schedule_id, bus_id, route_id, departure_time, arrival_time) VALUES (1, 1, 1, '09:00', '12:00');
-- INSERT INTO Schedule (schedule_id, bus_id, route_id, departure_time, arrival_time) VALUES (2, 2, 2, '10:00', '13:00');
-- INSERT INTO Schedule (schedule_id, bus_id, route_id, departure_time, arrival_time) VALUES (3, 3, 3, '11:00', '14:00');
-- SELECT * FROM Route;
-- INSERT INTO Schedule (schedule_id, bus_id, route_id, departure_time, arrival_time) VALUES (1, 1, 1, '09:00', '12:00');
-- ALTER TABLE Schedule ADD COLLUMN arrival_time TEXT;
-- INSERT INTO Schedule (schedule_id, bus_id, route_id, departure_time, arrival_time) VALUES (1, 1, 1, '09:00', '12:00');
-- SELECT * FROM Schedule;
-- CREATE TABLE schedule_backup (
--     schedule_id INTEGER PRIMARY KEY,
--     bus_id INTEGER,
--     route_id INTEGER,
--     departure_time TEXT,
--     arrival_time TEXT,
--     FOREIGN KEY (bus_id) REFERENCES Bus(bus_id),
--     FOREIGN KEY (route_id) REFERENCES Route(route_id)
-- );
-- INSERT INTO schedule_backup (schedule_id, bus_id, route_id, departure_time)
-- SELECT schedule_id, bus_id, route_id, departure_time FROM Schedule;
-- DROP TABLE Schedule;
-- ALTER TABLE schedule_backup RENAME TO Schedule;
-- INSERT INTO Schedule (schedule_id, bus_id, route_id, departure_time, arrival_time) VALUES (1, 1, 1, '09:00', '12:00');
-- INSERT INTO Schedule (schedule_id, bus_id, route_id, departure_time, arrival_time) VALUES (2, 2, 2, '10:00', '13:00');
-- INSERT INTO Schedule (schedule_id, bus_id, route_id, departure_time, arrival_time) VALUES (3, 3, 3, '11:00', '14:00');
-- INSERT INTO Ticket (ticket_id, passenger_id, bus_id, schedule_id, seat_number, journey_date) VALUES (1, 1, 1, 1, 'A1', '2023-06-01');
-- INSERT INTO Ticket (ticket_id, passenger_id, bus_id, schedule_id, seat_number, journey_date) VALUES (2, 2, 1, 2, 'B5', '2023-06-02');
-- INSERT INTO Ticket (ticket_id, passenger_id, bus_id, schedule_id, seat_number, journey_date) VALUES (3, 3, 2, 1, 'C3', '2023-06-03');
-- CREATE TABLE passenger_backup AS SELECT * FROM passenger;
-- DROP TABLE Passenger;
-- CREATE TABLE Passenger (
--     passenger_id INTEGER PRIMARY KEY AUTOINCREMENT,
--     name TEXT NOT NULL,
--     contact TEXT NOT NULL,
--     email TEXT NOT NULL
-- );
-- INSERT INTO Passenger (name, contact, email)
-- SELECT name, contact, email
-- FROM passenger_backup;
-- DROP TABLE passenger_backup;
-- INSERT INTO Payment(passenger_id,amount,payment_date,payment_method) VALUES (2,10.0,'10/02/2023','card');
-- INSERT INTO Passenger(name,contact,email) VALUES ('Igor','8800553535','sa;leka@gmail.com');
-- DELETE FROM Payment WHERE payment_id = 1;
-- DELETE FROM Payment WHERE passenger_id = 1;
-- SELECT * FROM Payment;
-- SELECT passenger_id FROM Payment WHERE type = 'table' AND sql LIKE '%FOREIGN KEY%';
-- ALTER TABLE Payment DROP CONSTRAINT fk_passenger_id;
-- CREATE TABLE Payemnt_backup AS SELECT * FROM Payment;
-- DROP TABLE Payment;
-- CREATE TABLE Payment (
--   payment_id INTEGER PRIMARY KEY AUTOINCREMENT,
--   passenger_id INTEGER,
--   amount DECIMAL,
--   payment_date TEXT,
--   payment_method TEXT,
--   FOREIGN KEY (passenger_id) REFERENCES Passenger (passenger_id)
-- );

-- INSERT INTO Payment (payment_id,passenger_id,amount,payment_date,payment_method)
-- SELECT payment_id,passenger_id,amount,payment_date,payment_method
-- FROM Payemnt_backup;

-- DROP TABLE Payemnt_backup;
-- SELECT * FROM Payment;
-- INSERT INTO Payment (passenger_id, amount, payment_date, payment_method)
-- VALUES (1, 250.00, '2023-05-21', 'Card');

-- ALTER TABLE Passenger RENAME COLUMN name TO fname;
-- ALTER TABLE Passenger ADD NEW COLLUMN fname TEXT;
-- ALTER TABLE Passenger ADD CONSTRAINT fk_lname UNIQUE(lname);
-- CREATE TABLE passenger_backup AS SELECT * FROM Passenger;
-- DROP TABLE Passenger;
-- CREATE TABLE Passenger (
--     passenger_id INTEGER PRIMARY KEY AUTOINCREMENT,
--     fname TEXT NOT NULL,
--     lname TEXT NOT NULL UNIQUE,
--     contact TEXT NOT NULL,
--     email TEXT NOT NULL
-- );
-- INSERT INTO Passenger (fname, contact, email)
-- SELECT fname,contact, email
-- FROM Passenger_backup;
-- DROP TABLE Passenger_backup;
-- SELECT * FROM Passenger;
-- INSERT INTO Passenger(fname,lname,contact,email) VALUES('Bob','Ferguson','559200245','qwerty@gmail.com');
-- SELECT * FROM Passenger;
-- DELETE FROM Passenger WHERE passenger_id = 1;
-- SELECT * FROM Passenger;
-- DELETE FROM Passenger WHERE passenger_id = 2;
-- INSERT INTO Passenger(fname,lname,contact,email) VALUES('Bilol','Makhmudov','5838889977','bilolmakh@gmail.com');
-- INSERT INTO Passenger(fname,lname,contact,email) VALUES('Nazar','Oskonbaev','5831112233','nazarosk@gmail.com');
-- Step 1: Create a new table without the column
-- CREATE TABLE Schedule (
--     schedule_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
--     departure_point TEXT NOT NULL,
--     destination_point TEXT NOT NULL,
--     travel_date TEXT NOT NULL,
--     -- Add other columns excluding the column to be deleted
--     departure_time TEXT NOT NULL,
--     destination_time TEXT NOT NULL
-- );

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Girne', 'Famagusta', '2023-06-16', '09:00', '10:30');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Lefcosia', 'Girne', '2023-06-16', '09:00', '09:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Lefcosia', 'Famagusta', '2023-06-16', '09:00', '09:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Famagusta', 'Girne', '2023-06-16', '09:00', '10:30');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Famagusta', 'Lefcosia', '2023-06-16', '09:00', '09:45');


-- -- Step 3: Delete the old table
-- DROP TABLE Schedule;

-- -- Step 4 (optional): Rename the new table to the original table name
-- ALTER TABLE Schedule_backup RENAME TO Schedule;
-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Girne', 'Lefcosia', '2023-06-16', '12:00', '12:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Girne', 'Famagusta', '2023-06-16', '12:00', '13:30');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Lefcosia', 'Girne', '2023-06-16', '12:00', '12:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Lefcosia', 'Famagusta', '2023-06-16', '12:00', '12:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Famagusta', 'Girne', '2023-06-16', '12:00', '13:30');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Famagusta', 'Lefcosia', '2023-06-16', '12:00', '12:45');


-- DELETE FROM Schedule WHERE schedule_id >= 14 AND schedule_id <= 17;
-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Girne', 'Lefcosia', '2023-06-16', '15:00', '15:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Girne', 'Famagusta', '2023-06-16', '15:00', '16:30');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Lefcosia', 'Girne', '2023-06-16', '15:00', '15:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Lefcosia', 'Famagusta', '2023-06-16', '15:00', '15:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Famagusta', 'Girne', '2023-06-16', '15:00', '16:30');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Famagusta', 'Lefcosia', '2023-06-16', '15:00', '15:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Girne', 'Lefcosia', '2023-06-16', '18:00', '18:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Girne', 'Famagusta', '2023-06-16', '18:00', '19:30');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Lefcosia', 'Girne', '2023-06-16', '18:00', '18:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Lefcosia', 'Famagusta', '2023-06-16', '18:00', '18:45');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Famagusta', 'Girne', '2023-06-16', '18:00', '19:30');

-- INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES ('Famagusta', 'Lefcosia', '2023-06-16', '18:00', '18:45');


-- SELECT * FROM Route;

-- SELECT name FROM sqlite_master WHERE type='table';
-- select * from bus;  
-- CREATE TABLE Schedule_backup (
--     schedule_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
--     bus_id  INTEGER NOT NULL,
--     departure_point TEXT NOT NULL,
--     destination_point TEXT NOT NULL,
--     travel_date TEXT NOT NULL,
--     -- Add other columns excluding the column to be deleted
--     departure_time TEXT NOT NULL,
--     destination_time TEXT NOT NULL,
--     FOREIGN KEY(bus_id) REFERENCES bus(bus_id)
-- );
-- DROP TABLE Schedule;
-- ALTER TABLE Schedule_backup RENAME TO Schedule;

-- INSERT INTO Schedule (bus_id, departure_point, destination_point, travel_date, departure_time, destination_time)
-- VALUES (2, 'Lefcosia', 'Girne', '2023-06-16', '18:00', '18:45');


-- select * from Passenger;

-- ticket_id, passenger_id, seat_number, schedule_id, modification_id,
-- INSERT INTO Ticket (passenger_id, seat_number, schedule_id, modification_date)
-- VALUES (3, 3, 169, DATE('now'));

-- select * from Schedule;
-- select * from Ticket;
-- select * from Passenger;


