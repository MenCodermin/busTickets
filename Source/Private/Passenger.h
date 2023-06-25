#pragma once

#include <string>
#include <sqlite3.h>

class Passenger
{
public:
    void RegisterPassenger(sqlite3* db);
    int GetPassengerID(sqlite3* db);

    

private:
    std::string fname;
    std::string lname;
    std::string contact;
    std::string email;
};