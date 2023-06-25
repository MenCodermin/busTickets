#include "Payment.h"
#include "Passenger.h"
#include "common.h"
#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>

using namespace std;

void Payment::ProcessPayment(sqlite3* db)
{
    int PaymentType, rc;
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
    case 1: // Payment By Cash
        cout << "Enter cash" << endl;

        break;
    case 2: // Payment By Card
        cout << "Card Holder:" << endl;
        getline(cin, cardHolder);
        cout << "Card number:" << endl;
        getline(cin, cardNo);
        cout << "Expericy date:" << endl;
        getline(cin, expiricyDate);
        cout << "CVV:" << endl;
        getline(cin, cvv);

        sql = "INSERT INTO Payment (passenger_id,amount,payment_date,payment_method) VALUES(?,?,?,?);";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr);
        if (rc != SQLITE_OK)
        {
            cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_int(statement, 1, PasObj.GetPassengerID(db));
        if (rc != SQLITE_OK)
        {
            cout << "Failed to bind passenger_id: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_double(statement, 2, amount);
        if (rc != SQLITE_OK)
        {
            cout << "Failed to bind amount: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement, 3, getCurrentDate().c_str(), -1, SQLITE_STATIC);
        if (rc != SQLITE_OK)
        {
            cout << "Failed to bind payment_date: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement, 4, paymethod[0].c_str(), -1, SQLITE_STATIC);
        if (rc != SQLITE_OK)
        {
            cout << "Failed to bind payment_method: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_step(statement);
        if (rc != SQLITE_DONE)
        {
            cout << "Failed to proceed the payment: " << sqlite3_errmsg(db) << endl;
            return;
        }

        sqlite3_finalize(statement);
        cout << "Payment inserted successfully!" << endl;
        break;
    case 3:
        break;
    default:
        cout << "Wrong enter!" << endl;
        break;
    }
}
