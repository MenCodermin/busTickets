#include <iostream>

#include "Ticket.h"
#include "Schedule.h"
#include "Passenger.h"
#include "Payment.h"

using namespace std;


int main()
{
    string departurePoint, destinationPoint;
    sqlite3* db;
    Payment pay;
    Ticket tic;
    Passenger pas;
    Schedule sc;

    char* errMsg = nullptr;
    int rc = sqlite3_open("bussys.db", &db);

    if (rc != SQLITE_OK)
    {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }
    

    // pas.RegisterPassenger(db);
    // pay.ProcessPayment(db);

    // departurePoint = chooseDeparturePoint();
//     destinationPoint = chooseDestinationPoint(departurePoint);
//    sc.GetAvailableSchedules(db,  departurePoint, destinationPoint);
   
    // AddSevenNewSchedule(db); //Adds new schedule for upcoming 7 days.
    // DeleteExpiredSchedules(db); //Deletes all the expired schedules. 
    // CancellationModificationProcess(db);


    sqlite3_close(db);
    std::cin.get();
    return 0;
}