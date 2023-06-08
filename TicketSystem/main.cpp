#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <vector>
#include <utility>
#include "lib/sqlite3.h"

using namespace std;


string saveLastName = "";

// TODO: Implement all functions. Responsible for this is Nazar
class Payment
{
public:
    void ProcessPayment(sqlite3* db);
    void ValidatePayment();

private:
    double amount;
    string cardNo;
    string cardHolder;
    string expiricyDate;
    string cvv;
    vector<string> paymethod {"card","cash"};
};

// TODO: Implement all functions. Responsible for this is Nazar
class Ticket
{
public:
    void CreateTicket();
    void PrintTicket();
    void CancelTicket();
};

// TODO: Implement all functions. Responsible for this is Nazar
class Passenger
{
public:
    void RegisterPassenger(sqlite3* db);
    void ViewBookingHistory(sqlite3* db);
    int GetPassengerID(sqlite3* db);

private:
    string fname;
    string lname;
    string contact;
    string email;
};

class Bus
{
public:
    void GetAvailableSeats();
    void BookSeats();
};

// TODO: Implement all functions. Responsible for this is Bilol
class Route
{
public:
    void GetRouteDetails();
    void CalculateDistance();
};

// TODO: Implement all functions. Responsible for this is Bilol
class Schedule
{
public:
    void GetAvailableSchedules();
    void UpdateSchedule();
};

string getCurrentDate()
{
    time_t currentTime = time(nullptr);
    tm* localTime = localtime(&currentTime);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localTime);

    return string(buffer);
}

int main()
{
    sqlite3* db;
    Payment pay;
    Ticket tic;
    Passenger pas;
    Bus b;
    Route r;
    Schedule sc;

    char* errMsg = nullptr;
    int rc = sqlite3_open("bussys.db", &db);

    if (rc != SQLITE_OK)
    {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }
    pas.RegisterPassenger(db);
    pay.ProcessPayment(db);
    cin.get();

    sqlite3_close(db);

    return 0;
}

void Ticket::CreateTicket() {}

void Passenger::RegisterPassenger(sqlite3* db)
{
    cout << "Enter passenger first name:" << endl;
    getline(cin, fname);

    cout << "Enter passenger last name:" << endl;
    getline(cin, lname);

    cout << "Enter contact information:" << endl;
    getline(cin, contact);

    cout << "Enter email:" << endl;
    getline(cin, email);

    string sql = "INSERT INTO Passenger (fname,lname, contact,email) VALUES ('" + fname + "','" + lname + "','" + contact + "','" + email + "');";
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

    if (fname.empty() || lname.empty() || contact.empty() || email.empty())
    {
        cout << "Invalid name or contact information. Registration failed." << std::endl;
        return;
    }

    if (rc != SQLITE_OK)
        cerr << "Error inserting passenger details into database: " << sqlite3_errmsg(db) << std::endl;
    else
        cout << "Passenger registration successful!" << std::endl;
    saveLastName = lname;
}

void Passenger::ViewBookingHistory(sqlite3* db) {}

int Passenger::GetPassengerID(sqlite3* db)
{
    string sql = "SELECT passenger_id FROM Passenger WHERE lname = ?;";

    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        return -1;
    }

    if (sqlite3_bind_text(statement, 1, saveLastName.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        cerr << "Error binding parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return -1;
    }

    int result = sqlite3_step(statement);
    if (result != SQLITE_ROW)
    {
        if (result == SQLITE_DONE)
        {
            cerr << "No matching rows found." << endl;
        }
        else
        {
            cerr << "Error executing statement: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(statement);
        return -1;
    }

    int passengerId = sqlite3_column_int(statement, 0);
    sqlite3_finalize(statement);

    return passengerId;
}

// Proceeding payment either by cash or by card
void Payment::ProcessPayment(sqlite3* db)
{
    int PaymentType,rc;
    string sql;
    Passenger PasObj;
    sqlite3_stmt* statement;
    cout << "Choose the method of the payment:\n"
         << "1. By Cash\n"
         << "2. By Card\n"
         << "3. Go Back" << endl;
    cin >> PaymentType;
    cin.ignore();

    switch (PaymentType)
    {
    case 1: //Payment By Cash
        cout << "Enter cash" << endl;

        break;
    case 2: //Payment By Card
        cout << "Card Holder:" << endl;
        getline(cin, cardHolder);
        cout << "Card number:" << endl;
        getline(cin, cardNo);
        cout << "Expericy date:" << endl;
        getline(cin, expiricyDate);
        cout << "CVV:" << endl;
        getline(cin, cvv);

        sql = "INSERT INTO Payment (passenger_id,amount,payment_date,payment_method) VALUES(?,?,?,?);";
        rc = sqlite3_prepare_v2(db,sql.c_str(), -1, &statement,nullptr);
        if (rc != SQLITE_OK) {
            cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_int(statement,1,PasObj.GetPassengerID(db));
        if (rc != SQLITE_OK) {
            cout << "Failed to bind passenger_id: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_double(statement,2,amount);
        if (rc != SQLITE_OK) {
            cout << "Failed to bind amount: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement,3,getCurrentDate().c_str(), -1, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            cout << "Failed to bind payment_date: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement,4,paymethod[0].c_str(),-1,SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            cout << "Failed to bind payment_method: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_step(statement);
        if (rc != SQLITE_DONE) {
            cout << "Failed to proceed the payment: " << sqlite3_errmsg(db) << endl;
            return;
        }

        sqlite3_finalize(statement);
        cout<<"Payment inserted successfully!"<<endl;
        break;
    case 3:
        break;
    default:
        cout << "Wrong enter!" << endl;
        break;
    }
}