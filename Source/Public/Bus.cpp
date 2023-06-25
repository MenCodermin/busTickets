#include "Source/Private/Bus.h"
#include "Source/Private/Schedule.h"
#include "Source/Private/Passenger.h"
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

void Bus::BookSeats(sqlite3* db)
{
    Passenger pas;
    string sql;
    sqlite3_stmt* statement;
    int bookSeat;
    set<int> bookedSeats = GetAvailableSeats(db);
    vector<string> seatStatus(totalSeats, "");

    for (int row = 1; row <= rows; ++row)
    {
        for (int seat = 1; seat <= seatsPerRow; ++seat)
        {
            int seatNumber = (row - 1) * seatsPerRow + seat;

            // Check if the seat is booked
            if (bookedSeats.count(seatNumber) > 0)
            {
                seatStatus[seatNumber - 1] = "x";
            }
            else
            {
                seatStatus[seatNumber - 1] = to_string(seatNumber);
            }
        }
    }

    // Display seat availability
    for (int i = 0; i < totalSeats; ++i)
    {
        cout << seatStatus[i] << " ";
        if ((i + 1) % 2 == 0)
            cout << "\t";

        if ((i + 1) % seatsPerRow == 0)
            cout << endl << endl;
    }

    cout << "Select the seat to book: ";
    cin >> bookSeat;

    sql = "INSERT INTO Booked_seats(bus_id,passenger_id,booked_seat,departure_date,departure_time) VALUES(?,?,?,?,?);";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind values to the statement with error checking
    if (sqlite3_bind_int(statement, 1, getCurrentBusId(db)) != SQLITE_OK)
    {
        cerr << "Error binding bus_id: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return;
    }

    if (sqlite3_bind_int(statement, 2, pas.GetPassengerID(db)) != SQLITE_OK)
    {
        cerr << "Error binding passenger_id: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return;
    }

    if (sqlite3_bind_int(statement, 3, bookSeat) != SQLITE_OK)
    {
        cerr << "Error binding booked_seat: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return;
    }

    if (sqlite3_bind_text(statement, 4, departureDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        cerr << "Error binding departure_date: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return;
    }

    if (sqlite3_bind_text(statement, 5, departureTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        cerr << "Error binding departure_time: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return;
    }

    if (sqlite3_step(statement) != SQLITE_DONE)
    {
        cerr << "Error executing statement: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(statement);
}
