#include <iostream>
#include "lib/sqlite3.h"

using namespace std;

// TODO: Implement all functions. Responsible for this is Nazar
class Payment   
{
public:
    void ProcessPayment();
    void ValidatePayment();
};

// TODO: Implement all functions. Responsible for this is Nazar
class Ticket
{
public:
    void CreateTicket();
    void PrintTicket();
    void CancelTicket();    
};

class Passenger
{
public:
    void RegisterPassenger();
    void ViewBookingHistory();
    
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

int main() {
    sqlite3* db;
    Payment pay;
    Ticket tic;
    Passenger pas;
    Bus b;
    Route r;
    Schedule sc;

    char* errMsg = nullptr;
    int rc = sqlite3_open("bussys.db",&db);

    if (rc != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }



    cin.get();

    sqlite3_close(db);

    return 0;
}


void Payment::ProcessPayment()
{
    
}


