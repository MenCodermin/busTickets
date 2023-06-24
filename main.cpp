#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <chrono>
#include <regex>
#include <string>
#include <map>
#include <cstring>
#include <set>
#include <ctime>
#include <vector>
#include <utility>
#include "lib/sqlite3.h"
#include "Passenger.h"
#include "Payment.h"
#include "Bus.h"
#include "Ticket.h"
#include "Source/Private/Schedule.h"
#include "common.h"

using namespace std;

string saveLastName = "";
string departureTime = "02:59", departureDate = "2023-06-18";
const int totalSeats = 48, rows = 12, seatsPerRow = 4;
Schedule sched;

class Payment
{
public:
    void ProcessPayment(sqlite3* db);

private:
    double amount;
    string cardNo;
    string cardHolder;
    string expiricyDate;
    string cvv;
    vector<string> paymethod{"card", "cash"};
};

class Ticket
{
public:
    void CreateTicket();
    void PrintTicket();
    void CancelTicket();
};

class Bus
{
public:
    set<int> GetAvailableSeats(sqlite3* db);
    void BookSeats(sqlite3* db);
};

class Schedule
{
public:
    void GetAvailableSchedules(sqlite3* db);
    void UpdateSchedule();
};

string getCurrentDate();
// string chooseDate();
// string chooseDeparturePoint();
string getCurrentTime();
int getCurrentBusId(sqlite3* db);
vector<string> getDepartureTime(sqlite3* db, const string& departurePoint, const string& destinationPoint);
vector<string> comingSevenDays();
string calculateDestinationTime(string& departureTime, string& departurePoint, string& destinationPoint);
void AddSevenNewSchedule(sqlite3* db);

int main()
{
    sqlite3* db;
    Payment pay;
    Ticket tic;
    Passenger pas;
    Bus b;
    Schedule sc;

    char* errMsg = nullptr;
    int rc = sqlite3_open("bussys.db", &db);

    if (rc != SQLITE_OK)
    {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    // AddSevenNewSchedule(db);
    pas.RegisterPassenger(db);
    b.BookSeats(db);

    cin.get();

    sqlite3_close(db);

    return 0;
}

void Ticket::CreateTicket() {}

string getCurrentDate()
{
    time_t currentTime = time(nullptr);
    tm* localTime = localtime(&currentTime);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localTime);

    return string(buffer);
}

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

void Bus::BookSeats(sqlite3* db)
{
    Passenger pas;
    string sql;
    sqlite3_stmt* statement;
    int bookSeat;
    set<int> bookedSeats = Bus::GetAvailableSeats(db);
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

string getCurrentTime()
{

    time_t currentTime = std::time(nullptr);
    tm* localTime = std::localtime(&currentTime);

    char buffer[6]; // Buffer to hold the formatted time
    std::strftime(buffer, sizeof(buffer), "%H:%M", localTime);

    return std::string(buffer);
}

string chooseDeparturePoint()
{
    string cities[] = {"Girne", "Lefcosia", "Famagusta"};
    int cityIndex;

    cout << "============ Departure Points ============" << endl;
    cout << "Choose your departure point:" << endl;
    for (int i = 0; i < 3; i++)
    {
        cout << "   " << i + 1 << ") " << cities[i] << endl;
    }
    cout << "==========================================" << endl;

    cin >> cityIndex;
    if (cityIndex >= 1 && cityIndex <= 3)
    {
        return cities[cityIndex - 1];
    }
    else
    {
        cout << "Invalid city index selected." << endl;
        return ""; // Return an empty string to indicate an error
    }
}

string chooseDestinationPoint(const string& departurePoint)
{
    vector<string> cities = {"Girne", "Lefcosia", "Famagusta"};

    // Find the departure point in the vector
    auto targetIter = find(cities.begin(), cities.end(), departurePoint);
    if (targetIter != cities.end())
    {
        // Erase the departure point from the vector
        cities.erase(targetIter);
    }
    else
    {
        cout << "Departure point not found in the cities list." << endl;
        return ""; // Return an empty string to indicate an error
    }

    // Display an attractive menu
    cout << "============ Destination Points ============" << endl;
    cout << "Choose your destination point:" << endl;
    for (size_t i = 0; i < cities.size(); i++)
    {
        cout << "   " << i + 1 << ") " << cities[i] << endl;
    }
    cout << "===================    =========================" << endl;

    int cityIndex;
    cin >> cityIndex;
    if (cityIndex >= 1 && cityIndex <= static_cast<int>(cities.size()))
    {
        return cities[cityIndex - 1];
    }
    else
    {
        cout << "Invalid city index selected." << endl;
        return ""; // Return an empty string to indicate an error
    }
}

void GetAvailableSchedules(sqlite3* db)
{
    vector<string> depTimeOptions;
    int depTimeIndex;
    string departurePoint = chooseDeparturePoint();
    string destinationPoint = chooseDestinationPoint(departurePoint);

    depTimeOptions = getDepartureTime(db, departurePoint, destinationPoint);
    if (!depTimeOptions.empty())
    {
        cout << "============ Available Departure Times ============" << endl;
        cout << "Choose the time you want to travel:" << endl;
        for (size_t i = 0; i < depTimeOptions.size(); i++)
        {
            cout << "   " << i + 1 << ") " << depTimeOptions[i] << endl;
        }
        cout << "==================================================" << endl;
    }
    else
    {
        cout << "There are no available buses for the chosen time. Please "
                "choose another day."
             << endl;
        GetAvailableSchedules(db);
        return;
    }

    cin >> depTimeIndex;

    if (depTimeIndex >= 1 && depTimeIndex <= static_cast<int>(depTimeOptions.size()))
    {
        string chosenDepartureTime = depTimeOptions[depTimeIndex - 1];
        departureTime = chosenDepartureTime;
        cout << "You have chosen the departure time: " << chosenDepartureTime << endl;
    }
    else
    {
        cout << "Invalid departure time index selected." << endl;
    }
}

vector<string> getDepartureTime(sqlite3* db, const string& departurePoint, const string& destinationPoint)
{
    vector<string> depTimeOptions;
    string travelDate = sched.chooseDate();
    departureDate = travelDate;
    string sql = "SELECT departure_time FROM Schedule WHERE departure_point "
                 "= ? AND destination_point = ? AND travel_date = ?;";
    sqlite3_stmt* statement;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        return {};
    }

    if (sqlite3_bind_text(statement, 1, departurePoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(statement, 2, destinationPoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(statement, 3, travelDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        cerr << "Error binding parameters: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return {};
    }

    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        const unsigned char* departureTime = sqlite3_column_text(statement, 0);
        string strDepartureTime(reinterpret_cast<const char*>(departureTime));

        if (travelDate != getCurrentDate() || strDepartureTime > getCurrentTime())
        {
            depTimeOptions.push_back(strDepartureTime);
        }
    }

    sqlite3_finalize(statement);
    return depTimeOptions;
}

int getCurrentBusId(sqlite3* db)
{
    string sql;
    sql = "SELECT bus_id FROM Schedule WHERE departure_time = ? AND travel_date = ?;";

    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    if (sqlite3_bind_text(statement, 1, departureTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(statement, 2, departureDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        cerr << "Error binding parameters: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return 0;
    }
    int bus_id = sqlite3_step(statement);
    sqlite3_finalize(statement);
    return bus_id;
}

string calculateDestinationTime(string& departureTime, string& departurePoint, string& destinationPoint)
{
    std::tm tm = {};
    std::istringstream ss(departureTime);
    ss >> std::get_time(&tm, "%H:%M");
    int toOneSide;
    if (departurePoint == "Girne" && destinationPoint == "Lefcosia")
    {
        toOneSide = 30;
    }
    else if (departurePoint == "Girne" && destinationPoint == "Famagusta")
    {
        toOneSide = 90;
    }
    else if (departurePoint == "Lefcosia" && destinationPoint == "Girne")
    {
        toOneSide = 30;
    }
    else if (departurePoint == "Lefcosia" && destinationPoint == "Famagusta")
    {
        toOneSide = 60;
    }
    else if (departurePoint == "Famagusta" && destinationPoint == "Girne")
    {
        toOneSide = 90;
    }
    else if (departurePoint == "Famagusta" && destinationPoint == "Lefcosia")
    {
        toOneSide = 60;
    }

    auto time = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    time += std::chrono::minutes(toOneSide);

    std::time_t destinationTime = std::chrono::system_clock::to_time_t(time);
    std::tm* destinationTm = std::localtime(&destinationTime);

    std::ostringstream oss;
    oss << put_time(destinationTm, "%H:%M");

    return oss.str();
}

vector<string> comingSevenDays()
{
    time_t currentTime = time(nullptr);
    vector<string> days(7);
    int input;

    for (int i = 0; i < 7; ++i)
    {
        tm* date = localtime(&currentTime);
        date->tm_mday += i;

        mktime(date);

        int year = date->tm_year + 1900; // Year since 1900
        int month = date->tm_mon + 1;    // Month since January (0-based)
        int day = date->tm_mday;         // Day of the month

        // Format the day as "yyyy-mm-dd"
        stringstream ss;
        ss << year << "-" << std::setfill('0') << std::setw(2) << month << "-" << std::setfill('0') << std::setw(2) << day;
        days[i] = ss.str();
    }
    return days;
}