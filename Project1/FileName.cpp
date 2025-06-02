#include <iostream>
#include <string>
#include <iomanip>
#include <conio.h>
#include <memory>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;
using namespace sql;

class Database {
private:
    Driver* driver;
    unique_ptr<Connection> conn;
public:
    Database() {
        driver = get_driver_instance();
        conn.reset(driver->connect("tcp://127.0.0.1:3306", "root", ""));
        conn->setSchema("Hotel");
    }
    Connection* getConnection() {
        return conn.get();
    }
};

class User {
protected:
    Connection* con;
    User(Connection* connection) : con(connection) {}
    string inputPassword(const string& prompt = "Password") {
        string pwd;
        char ch;
        cout << prompt << ": ";
        while ((ch = _getch()) != '\r') {
            if (ch == '\b') {
                if (!pwd.empty()) {
                    pwd.pop_back();
                    cout << "\b \b";
                }
            }
            else {
                pwd.push_back(ch);
                cout << '*';
            }
        }
        cout << endl;
        return pwd;
    }
};

class Manager : public User {
public:
    Manager(Connection* connection) : User(connection) {}
    bool login() {
        string user;
        for (int i = 0; i < 3; ++i) {
            cout << "Username: ";
            getline(cin, user);
            string pwd = inputPassword("Password");
            if (user == "admin" && pwd == "1234")
                return true;
            cout << "Invalid credentials.\n";
        }
        cout << "Too many failed attempts. Exiting.\n";
        exit(1);
    }

    void addCustomer() {
        string name, id, phone, email;
        int age;
        int i;
        cin.ignore();
        cout << "Name: ";
        getline(cin, name);
        cout << "ID (14 digits): ";
        getline(cin, id);
        while (id.size() != 14) {
            cout << "ID must be 14 digits: ";
            getline(cin, id);
        }
        cout << "Age (>20): ";
        cin >> age;
        for (i = 0; age < 20 && i < 3; i++) {
            cout << "\nThe age should be more then 20 ";
            cin >> age;
        }
        if (i == 3) {
            cout << "Too many failed attempts. Exiting.\n";
            exit(1);
        }
        cin.ignore();
        cout << "Phone: ";
        getline(cin, phone);
        cout << "Email: ";
        getline(cin, email);
        unique_ptr<PreparedStatement> ps(con->prepareStatement(
            "INSERT INTO Customer(Name,ID,Age,Phone,Email) VALUES(?,?,?,?,?)"));
        ps->setString(1, name);
        ps->setString(2, id);
        ps->setInt(3, age);
        ps->setString(4, phone);
        ps->setString(5, email);
        ps->executeUpdate();
        cout << "Customer added.\n";
    }

    void showOptions() {
        unique_ptr<Statement> st(con->createStatement());
        unique_ptr<ResultSet> rs(st->executeQuery("SELECT OptionID, OptionName, Price FROM Options"));

        cout << "\n--- Available Options ---\n";
        while (rs->next()) {
            cout << "ID: " << rs->getInt("OptionID")
                << " | Name: " << rs->getString("OptionName")
                << " | Price: $" << fixed << setprecision(2) << rs->getDouble("Price") << "\n";
        }
    }
    void showHalls() {
        unique_ptr<Statement> st(con->createStatement());
        unique_ptr<ResultSet> rs(st->executeQuery("SELECT ID, Name, NumberOfPeople, Occasion FROM Hall"));

        cout << "\n--- Available Halls ---\n";
        while (rs->next()) {
            cout << "ID: " << rs->getInt("ID")
                << " | Name: " << rs->getString("Name")
                << " | Capacity: " << rs->getInt("NumberOfPeople")
                << " | Occasion: " << rs->getString("Occasion") << "\n";
        }
    }



    void addEmployee() {
        string name, id, pos; float sal;
        int i = 0;
        cin.ignore();
        cout << "Name: ";
        getline(cin, name);
        cout << "ID (14 digits): ";
        getline(cin, id);
        while (id.size() != 14 && i < 3) {
            i++;
            cout << "ID must be 14 digits: ";
            getline(cin, id);
        }
        if (i == 3) {
            cout << "Too many failed attempts. Exiting.\n";
            exit(1);
        }
        cout << "Position: ";
        getline(cin, pos);
        cout << "Salary: ";
        cin >> sal;
        unique_ptr<PreparedStatement> ps(con->prepareStatement(
            "INSERT INTO Employee(Name,ID,Position,Salary) VALUES(?,?,?,?)"));
        ps->setString(1, name); ps->setString(2, id);
        ps->setString(3, pos); ps->setDouble(4, sal);
        ps->executeUpdate(); cout << "Employee added.\n";
    }

    void addRoom() {
        int num, beds, people;
        double price;
        string bath;
        cout << "Room# :";
        cin >> num;
        cout << "Beds :";
        cin >> beds;
        cout << "People :";
        cin >> people;
        cout << "Price :";
        cin >> price;
        cin.ignore();
        cout << "Bathroom(inner/outer):";
        cin >> bath;
        unique_ptr<PreparedStatement> ps(con->prepareStatement(
            "INSERT INTO Room(Number,NumberOfbeds,NumberOfPeople,Price,Bathroom) VALUES(?,?,?,?,?)"));
        ps->setInt(1, num); ps->setInt(2, beds); ps->setInt(3, people);
        ps->setDouble(4, price); ps->setString(5, bath);
        ps->executeUpdate(); cout << "Room added.\n";
    }

    void addOption() {
        string name;
        double price;
        cin.ignore();
        cout << "Option Name:";
        getline(cin, name);
        cout << "Price :"; cin >> price;
        unique_ptr<PreparedStatement> ps(con->prepareStatement(
            "INSERT INTO Options(OptionName,Price) VALUES(?,?)"));
        ps->setString(1, name); ps->setDouble(2, price);
        ps->executeUpdate(); cout << "Option added.\n";
    }

    int lastInsertId() {
        unique_ptr<Statement> st(con->createStatement());
        unique_ptr<ResultSet> rs(st->executeQuery("SELECT LAST_INSERT_ID() AS id"));
        return rs->next() ? rs->getInt("id") : 0;
    }

    /* void makeReservation() {
         string cid;
         int room, days;
         cin.ignore();
         cout << "Customer ID:";
         getline(cin, cid);
         cout << "Room#:"; cin >> room;
         cout << "Days:"; cin >> days;
         unique_ptr<PreparedStatement> ps(con->prepareStatement(
             "INSERT INTO Reservation(CustomerID,RoomNumber,NumberOfDays,TotalPrice,Status)"
             " VALUES(?,?,?,(SELECT Price FROM Room WHERE Number=?)*?, 'active')"));
         ps->setString(1, cid);
         ps->setInt(2, room);
         ps->setInt(3, days);
         ps->setInt(4, room);
         ps->setInt(5, days);
         ps->executeUpdate();
         int rid = lastInsertId();

         char more;
         do {
             int oid, qty;
             cout << "Option ID:"; cin >> oid;
             cout << "Qty:"; cin >> qty;
             unique_ptr<PreparedStatement> po(con->prepareStatement(
                 "INSERT INTO ReservationOptions(ReservationID,OptionID,Quantity,OptionPrice)"
                 " VALUES(?,?,?,(SELECT Price FROM Options WHERE OptionID=?))"));
             po->setInt(1, rid);
             po->setInt(2, oid);
             po->setInt(3, qty);
             po->setInt(4, oid);
             po->executeUpdate();
             cout << "Add more options? (y/n):"; cin >> more;
         } while (more == 'y');

         char addHall;
         cout << "Add hall to reservation? (y/n): "; cin >> addHall;
         while (addHall == 'y') {
             int hallID, quantity;
             double price;
             cout << "Hall ID: ";
             cin >> hallID;
             cout << "Quantity: ";
             cin >> quantity;
             cout << "Price per Hall: ";
             cin >> price;
             unique_ptr<PreparedStatement> ph(con->prepareStatement(
                 "INSERT INTO ReservationHalls(ReservationID, HallID, quantity, price) VALUES(?,?,?,?)"));
             ph->setInt(1, rid);
             ph->setInt(2, hallID);
             ph->setInt(3, quantity);
             ph->setDouble(4, price);
             ph->executeUpdate();
             cout << "Add another hall? (y/n): "; cin >> addHall;
         }

         updateTotalPrice(rid);
         showReservation(rid);
     }*/
    void makeReservation() {
        string cid;
        int room, days;
        cin.ignore();





        cout << "Customer ID: "; getline(cin, cid);
        cout << "\n--- Available Rooms ---\n";
        unique_ptr<Statement> st(con->createStatement());
        unique_ptr<ResultSet> rs(st->executeQuery(
            "SELECT Number, NumberOfbeds, NumberOfPeople, Price, Bathroom FROM Room "
            "WHERE Number NOT IN (SELECT RoomNumber FROM Reservation WHERE Status = 'active')"));
        while (rs->next()) {
            cout << "Room#: " << rs->getInt("Number")
                << " | Beds: " << rs->getInt("NumberOfbeds")
                << " | Capacity: " << rs->getInt("NumberOfPeople")
                << " | Price: $" << rs->getDouble("Price")
                << " | Bathroom: " << rs->getString("Bathroom") << "\n";
        }
        cout << "Room#: "; cin >> room;
        cout << "Days: "; cin >> days;

        unique_ptr<PreparedStatement> ps(con->prepareStatement(
            "INSERT INTO Reservation(CustomerID,RoomNumber,NumberOfDays,TotalPrice,Status)"
            " VALUES(?,?,?,(SELECT Price FROM Room WHERE Number=?)*?, 'active')"));
        ps->setString(1, cid);
        ps->setInt(2, room);
        ps->setInt(3, days);
        ps->setInt(4, room);
        ps->setInt(5, days);
        ps->executeUpdate();

        int rid = lastInsertId();
        string moree;
        cout << "Do you want more ooption like picnic ,tour .... ?(yes/no)";
        cin >> moree;
        if (moree == "yes")
        {
            cout << endl;
            showOptions();
            char more;
            do {
                int oid, qty;
                cout << "Option ID: "; cin >> oid;
                cout << "Qty: "; cin >> qty;
                unique_ptr<PreparedStatement> po(con->prepareStatement(
                    "INSERT INTO ReservationOptions(ReservationID,OptionID,Quantity,OptionPrice)"
                    " VALUES(?,?,?,(SELECT Price FROM Options WHERE OptionID=?))"));
                po->setInt(1, rid);
                po->setInt(2, oid);
                po->setInt(3, qty);
                po->setInt(4, oid);
                po->executeUpdate();
                cout << "Add more options? (y/n): "; cin >> more;
            } while (more == 'y');
        }


        char addHall;
        cout << "Add hall to reservation? (y/n): "; cin >> addHall;
        while (addHall == 'y') {
            cout << endl;
            showHalls();
            int hallID, quantity;
            double price;
            cout << "Hall ID: "; cin >> hallID;
            cout << "Quantity: "; cin >> quantity;
            cout << "Price per Hall: "; cin >> price;
            unique_ptr<PreparedStatement> ph(con->prepareStatement(
                "INSERT INTO ReservationHalls(ReservationID, HallID, quantity, price) VALUES(?,?,?,?)"));
            ph->setInt(1, rid);
            ph->setInt(2, hallID);
            ph->setInt(3, quantity);
            ph->setDouble(4, price);
            ph->executeUpdate();
            cout << "Add another hall? (y/n): "; cin >> addHall;
        }

        updateTotalPrice(rid);
        showReservation(rid);
    }


    void updateTotalPrice(int rid) {
        unique_ptr<Statement> st(con->createStatement());
        unique_ptr<ResultSet> rs(st->executeQuery(
            "SELECT r.NumberOfDays, rm.Price,"
            " IFNULL(SUM(ro.Quantity*ro.OptionPrice),0) AS optTotal,"
            " IFNULL((SELECT SUM(quantity*price) FROM ReservationHalls WHERE ReservationID = r.ReservationID),0) AS hallTotal"
            " FROM Reservation r"
            " JOIN Room rm ON r.RoomNumber=rm.Number"
            " LEFT JOIN ReservationOptions ro ON r.ReservationID=ro.ReservationID"
            " WHERE r.ReservationID=" + to_string(rid) +
            " GROUP BY r.NumberOfDays,rm.Price"));
        if (rs->next()) {
            double total = rs->getInt("NumberOfDays") * rs->getDouble("Price") + rs->getDouble("optTotal") + rs->getDouble("hallTotal");
            unique_ptr<PreparedStatement> pu(con->prepareStatement(
                "UPDATE Reservation SET TotalPrice=? WHERE ReservationID=?"));
            pu->setDouble(1, total);
            pu->setInt(2, rid);
            pu->executeUpdate();
        }
    }

    void showReservation(int rid) {
        unique_ptr<PreparedStatement> ps(con->prepareStatement(
            "SELECT r.ReservationID,c.Name, r.RoomNumber, r.NumberOfDays, r.TotalPrice, r.Status"
            " FROM Reservation r"
            " JOIN Customer c ON r.CustomerID=c.ID"
            " WHERE r.ReservationID=?"));
        ps->setInt(1, rid);
        unique_ptr<ResultSet> rs(ps->executeQuery());
        if (rs->next()) {
            cout << "ID:" << rs->getInt(1)
                << " Cust:" << rs->getString(2)
                << " Room:" << rs->getInt(3)
                << " Days:" << rs->getInt(4)
                << " Total:" << fixed << setprecision(2) << rs->getDouble(5)
                << " Status:" << rs->getString(6) << "\n";
        }
        unique_ptr<PreparedStatement> po(con->prepareStatement(
            "SELECT o.OptionName,ro.Quantity,ro.OptionPrice"
            " FROM ReservationOptions ro"
            " JOIN Options o ON ro.OptionID=o.OptionID"
            " WHERE ro.ReservationID=?"));
        po->setInt(1, rid);
        unique_ptr<ResultSet> ro(po->executeQuery());
        while (ro->next()) {
            cout << "  " << ro->getString(1)
                << " *" << ro->getInt(2)
                << " $" << fixed << setprecision(2) << ro->getDouble(3) << "\n";
        }
    }

    void cancelReservation() {
        int rid; cout << "Reservation ID:"; cin >> rid;
        unique_ptr<PreparedStatement> ps(con->prepareStatement("UPDATE Reservation SET Status='cancelled' WHERE ReservationID=?"));
        ps->setInt(1, rid); ps->executeUpdate();
        cout << "Cancelled.\n";
    }

    void checkoutReservation() {
        int rid; cout << "Reservation ID:"; cin >> rid;
        unique_ptr<PreparedStatement> ps(con->prepareStatement("UPDATE Reservation SET Status='checked_out' WHERE ReservationID=?"));
        ps->setInt(1, rid); ps->executeUpdate();
        cout << "Checked out.\n";
    }

    void searchReservation() {
        int rid; cout << "Reservation ID:"; cin >> rid;
        showReservation(rid);
    }

    void addhall() {
        int id, numberOfPeople;
        string name, occasion;
        cout << "Hall ID: "; cin >> id;
        cout << "Number Of People: "; cin >> numberOfPeople;
        cin.ignore();
        cout << "Name: "; getline(cin, name);
        cout << "Occasion: "; getline(cin, occasion);
        unique_ptr<PreparedStatement> ps(con->prepareStatement(
            "INSERT INTO Hall(ID, Name, NumberOfPeople, Occasion) VALUES(?, ?, ?, ?)"));
        ps->setInt(1, id);
        ps->setString(2, name);
        ps->setInt(3, numberOfPeople);
        ps->setString(4, occasion);
        ps->executeUpdate();
        cout << "Hall added.\n";
    }

    void menu() {
        int ch;
        do {
            cout << "\n1.Customer \n2.Employee \n3.Room \n4.Option \n5.Reserve \n6.Cancel \n7.Checkout \n8.Search \n9.Add hall\n0.Exit\n";
            cout << "Choice:"; cin >> ch;
            switch (ch) {
            case 1: addCustomer(); break;
            case 2: addEmployee(); break;
            case 3: addRoom(); break;
            case 4: addOption(); break;
            case 5: makeReservation(); break;
            case 6: cancelReservation(); break;
            case 7: checkoutReservation(); break;
            case 8: searchReservation(); break;
            case 9: addhall(); break;
            case 0: cout << "Bye\n"; break;
            default: cout << "Invalid\n"; break;
            }
        } while (ch != 0);
    }
};

int main() {
    try {
        Database db;
        Manager mgr(db.getConnection());
        if (mgr.login()) mgr.menu();
    }
    catch (SQLException& e) {
        cout << "SQL Error:" << e.what() << endl;
    }
    return 0;
}