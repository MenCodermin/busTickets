#include "Payment.h"
#include "Passenger.h"
#include <iostream>

using namespace std;

void Payment::ValidatePayment(){};

void Payment::ProcessPayment(sqlite3* db)
{
    int PaymentType,rc;
    string sql;
    Passenger PasObj;
    sqlite3_stmt* statement;
    std::cout << "Choose the method of the payment:\n"
         << "1. By Cash\n"
         << "2. By Card\n"
         << "3. Go Back" << endl;
    std::cin >> PaymentType;
    std::cin.ignore();

    switch (PaymentType)
    {
    case 1: //Payment By Cash
        std::cout << "Enter cash" << endl;

        break;
    case 2: //Payment By Card
        std::cout << "Card Holder:" << endl;
        std::getline(std::cin, cardHolder);
        std::cout << "Card number:" << endl;
        std::getline(std::cin, cardNo);
        std::cout << "Expericy date:" << endl;
        std::getline(std::cin, expiricyDate);
        std::cout << "CVV:" << endl;
        std::getline(std::cin, cvv);

        sql = "INSERT INTO Payment (passenger_id,amount,payment_date,payment_method) VALUES(?,?,?,?);";
        rc = sqlite3_prepare_v2(db,sql.c_str(), -1, &statement,nullptr);
        if (rc != SQLITE_OK) {
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_int(statement,1,PasObj.GetPassengerID(db));
        if (rc != SQLITE_OK) {
            std::cout << "Failed to bind passenger_id: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_double(statement,2,amount);
        if (rc != SQLITE_OK) {
            std::cout << "Failed to bind amount: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement,3,getCurrentDate().c_str(), -1, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            std::cout << "Failed to bind payment_date: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement,4,paymethod[0].c_str(),-1,SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            std::cout << "Failed to bind payment_method: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_step(statement);
        if (rc != SQLITE_DONE) {
            std::cout << "Failed to proceed the payment: " << sqlite3_errmsg(db) << endl;
            return;
        }

        sqlite3_finalize(statement);
        std::cout<<"Payment inserted successfully!"<<endl;
        break;
    case 3:
        break;
    default:
        std::cout << "Wrong enter!" << endl;
        break;
    }
}