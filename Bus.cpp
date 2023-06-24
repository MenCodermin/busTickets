#include "Source/Private/Bus.h"
#include "Source/Private/Schedule.h"
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <sqlite3.h>

using namespace std;

set<int> Bus::GetAvailableSeats(sqlite3* db)
{
    string sql;
    sqlite3_stmt* statement;

    set<int> bookedSeats;

    int bus_id = getCurrentBusId(db);

    sql = "SELECT booked_seat FROM Booked_seats WHERE bus_id = ? AND departure_time = ? AND departure_date = ?;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        return bookedSeats;
    }
    if (sqlite3_bind_int(statement, 3, bus_id) != SQLITE_OK ||
        sqlite3_bind_text(statement, 1, departureTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(statement, 2, departureDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        cerr << "Error binding parameters: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return bookedSeats;
    }
    while (sqlite3_step(statement) == SQLITE_OK)
    {
        int bookedSeat = sqlite3_column_int(statement, 0);
        bookedSeats.insert(bookedSeat);
    }
    sqlite3_finalize(statement);
    return bookedSeats;
}

int Bus::getCurrentBusId(sqlite3* db,string CurrentdepartureTime, string CurrentdepartureDate)
{
    string sql;
    sql = "SELECT bus_id FROM Schedule WHERE departure_time = ? AND travel_date = ?;";
    departureTime = CurrentdepartureTime;
    departureDate = CurrentdepartureDate;
    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    if (sqlite3_bind_text(statement, 1, CurrentdepartureTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(statement, 2, CurrentdepartureDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        cerr << "Error binding parameters: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return 0;
    }
    int bus_id = sqlite3_step(statement);
    sqlite3_finalize(statement);
    return bus_id;
}

