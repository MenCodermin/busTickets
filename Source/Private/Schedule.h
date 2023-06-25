#pragma once

#include "common.h"
#include "sqlite3.h"
#include <unordered_map>
#include <istream>
#include <iomanip>
#include <chrono>
#include <vector>

using namespace std;

class Schedule
{
public:
    void GetAvailableSchedules(sqlite3* db, const string& departurePoint, const string& destinationPoint);
    vector<string>comingSevenDays();

    void AddSevenNewSchedule(sqlite3* db);
    void DeleteExpiredSchedules(sqlite3* db);

    string chooseDate();
    string chooseDeparturePoint();

    string chooseDestinationPoint(const string& departurePoint);
    string calculateDestinationTime(string& departureTime, string& departurePoint, string& destinationPoint);
    
    void InsertNewSchedule(sqlite3* db, const string& departurePoint, const string& destinationPoint,
                       const string& travelDate, const string& departureTime, const string& destinationTime);
};
