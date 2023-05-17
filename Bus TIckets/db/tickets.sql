CREATE TABLE admin (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  fName TEXT NOT NULL,
  lName TEXT NOT NULL,
  position TEXT
);

CREATE TABLE passenger (
  pNum INTEGER PRIMARY KEY AUTOINCREMENT,
  fName TEXT NOT NULL,
  lName TEXT NOT NULL,
  address TEXT,
  citizenship TEXT,
  phone TEXT,
  email TEXT NOT NULL,
  password TEXT NOT NULL
);

CREATE TABLE trip (
  tripNum INTEGER PRIMARY KEY AUTOINCREMENT,
  admin INTEGER REFERENCES admin(id),
  contact TEXT,
  driver TEXT,
  depTime TEXT,
  arrivalTime TEXT,
  destination TEXT,
  departureFrom TEXT,
  plateNum TEXT
);

CREATE TABLE ticket (
  ticketNum INTEGER PRIMARY KEY AUTOINCREMENT,
  tripNum INTEGER REFERENCES trip(tripNum),
  pNum INTEGER REFERENCES passenger(pNum),
  id INTEGER REFERENCES admin(id)
);


-- Insert data into the admin table
INSERT INTO admin (fName, lName, position) VALUES
  ('John', 'Doe', 'Manager'),
  ('Jane', 'Smith', 'Supervisor');

-- Insert data into the passenger table
INSERT INTO passenger (fName, lName, address, citizenship, phone, email, password) VALUES
  ('Michael', 'Johnson', '123 Main St', 'USA', '123456789', 'michael@example.com', 'password123'),
  ('Sarah', 'Williams', '456 Elm St', 'Canada', '987654321', 'sarah@example.com', 'password456');

-- Insert data into the trip table
INSERT INTO trip (admin, contact, driver, depTime, arrivalTime, destination, departureFrom, plateNum) VALUES
  (1, 'John Smith', 'David Johnson', '2023-05-18 09:00:00', '2023-05-18 12:00:00', 'City A', 'City B', 'ABC123'),
  (2, 'Jane Doe', 'Robert Brown', '2023-05-19 10:00:00', '2023-05-19 13:00:00', 'City B', 'City C', 'DEF456');

-- Insert data into the ticket table
INSERT INTO ticket (tripNum, pNum, id) VALUES
  (1, 1, 1),
  (2, 2, 2);

SELECT * FROM admin;
SELECT * FROM passenger;
SELECT * FROM trip;
SELECT * FROM ticket;
