
# Hotel Management System (C++ with MySQL)

This is a command-line-based Hotel Management System developed in **C++** with **MySQL** for database storage. The system is designed for hotel managers to perform operations such as managing customers, employees, rooms, halls, options (like picnic or tour), and reservations.

## Features

- Admin login with username and password
- Add and manage:
  - Customers
  - Employees
  - Rooms
  - Options (services like picnic, tour, etc.)
  - Halls for occasions
- Make room reservations
- Add optional services and hall bookings to reservations
- Calculate total price automatically
- Search, cancel, and checkout reservations

## Technologies Used

- **C++**  
- **MySQL**  
- **MySQL Connector/C++**
- **Console-based UI**

## Prerequisites

- MySQL server installed and running
- MySQL Connector/C++ installed
- A MySQL database named `Hotel` with appropriate tables created
- Compiler that supports C++11 or later (e.g., g++, MSVC)

## Database Schema

You should have a MySQL database named `Hotel` with the following tables:

### `Customer`
```sql
CREATE TABLE Customer (
    ID VARCHAR(14) PRIMARY KEY,
    Name VARCHAR(100),
    Age INT,
    Phone VARCHAR(20),
    Email VARCHAR(100)
);
```

### `Employee`
```sql
CREATE TABLE Employee (
    ID VARCHAR(14) PRIMARY KEY,
    Name VARCHAR(100),
    Position VARCHAR(100),
    Salary DOUBLE
);
```

### `Room`
```sql
CREATE TABLE Room (
    Number INT PRIMARY KEY,
    NumberOfbeds INT,
    NumberOfPeople INT,
    Price DOUBLE,
    Bathroom VARCHAR(50)
);
```

### `Options`
```sql
CREATE TABLE Options (
    OptionID INT AUTO_INCREMENT PRIMARY KEY,
    OptionName VARCHAR(100),
    Price DOUBLE
);
```

### `Hall`
```sql
CREATE TABLE Hall (
    ID INT PRIMARY KEY,
    Name VARCHAR(100),
    NumberOfPeople INT,
    Occasion VARCHAR(100)
);
```

### `Reservation`
```sql
CREATE TABLE Reservation (
    ReservationID INT AUTO_INCREMENT PRIMARY KEY,
    CustomerID VARCHAR(14),
    RoomNumber INT,
    NumberOfDays INT,
    TotalPrice DOUBLE,
    Status VARCHAR(20),
    FOREIGN KEY (CustomerID) REFERENCES Customer(ID),
    FOREIGN KEY (RoomNumber) REFERENCES Room(Number)
);
```

### `ReservationOptions`
```sql
CREATE TABLE ReservationOptions (
    ID INT AUTO_INCREMENT PRIMARY KEY,
    ReservationID INT,
    OptionID INT,
    Quantity INT,
    OptionPrice DOUBLE,
    FOREIGN KEY (ReservationID) REFERENCES Reservation(ReservationID),
    FOREIGN KEY (OptionID) REFERENCES Options(OptionID)
);
```

### `ReservationHalls`
```sql
CREATE TABLE ReservationHalls (
    ID INT AUTO_INCREMENT PRIMARY KEY,
    ReservationID INT,
    HallID INT,
    Quantity INT,
    Price DOUBLE,
    FOREIGN KEY (ReservationID) REFERENCES Reservation(ReservationID),
    FOREIGN KEY (HallID) REFERENCES Hall(ID)
);
```

## Admin Login

- **Username**: `admin`  
- **Password**: `1234`

You have 3 attempts before the program exits.

## How to Compile

Make sure you link against the MySQL Connector library. Example using `g++`:

```sh
g++ main.cpp -o hotel -lmysqlcppconn
```

Or using Visual Studio, make sure to include the MySQL Connector/C++ headers and link the appropriate `.lib` files.

## How to Run

```sh
./hotel
```

## Usage

After logging in, you will be presented with a menu:

```
1. Customer  
2. Employee  
3. Room  
4. Option  
5. Reserve  
6. Cancel  
7. Checkout  
8. Search  
9. Add hall  
0. Exit
```

Enter the corresponding number to perform an action.

## Notes

- Input validations (like age, ID length) are enforced.
- Password input is hidden during typing.
- Total reservation price includes:
  - Room price × number of days
  - Total options selected
  - Total halls added
- You can search by reservation ID to view full details.

## License

This project is provided for educational purposes only.
