#include "Passenger.h"
#include <iostream>

using namespace std;

string saveLastName = "";
void Passenger::RegisterPassenger(sqlite3* db)
{
    std::cout << "Enter passenger first name:" << endl;
    std::getline(std::cin, fname);

    std::cout << "Enter passenger last name:" << endl;
    std::getline(std::cin, lname);

    std::cout << "Enter contact information:" << endl;
    std::getline(std::cin, contact);

bool enteredValidMail = false;
while(!enteredValidMail){
    std::cout << "Enter email:" << endl;
    std::getline(cin, email);
    if (isValidEmail(email)){
        std::cout<<"Your email has been recorded"<<endl;
        enteredValidMail = true;
    }
    else
    {
        std::cout<<"Please, enter a valid email\n"<<endl;
    }
}
    string sql = "INSERT INTO Passenger (fname,lname, contact,email) VALUES ('" + fname + "','" + lname + "','" + contact + "','" + email + "');";
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

    if (fname.empty() || lname.empty() || contact.empty() || email.empty())
    {
        std::cout << "Invalid name or contact information. Registration failed." << std::endl;
        return;
    }

    if (rc != SQLITE_OK)
        cerr << "Error inserting passenger details into database: " << sqlite3_errmsg(db) << std::endl;
    else
        std::cout << "Passenger registration successful!" << std::endl;
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