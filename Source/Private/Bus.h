#include <iostream>
#include <set>
#include <sqlite3.h>

using namespace std;

class Bus
{
public:
    std::set<int> GetAvailableSeats(sqlite3* db);
    int getCurrentBusId(sqlite3* db,string CurrentdepartureTime, string CurrentdepartureDate);
    void BookSeats(sqlite3* db);
    
private:
    int getCurrentBusId(sqlite3* db);
    string departureTime;
    string departureDate;
    const int totalSeats = 48, rows = 12,seatsPerRow = 4;

};