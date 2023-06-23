#include <iostream>
#include "lib/sqlite3.h"

#include "common.h"
using namespace std;


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