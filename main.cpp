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

using namespace std;

string saveLastName = "";
string departureTime = "02:59", departureDate = "2023-06-18";
const int totalSeats = 48, rows = 12, seatsPerRow = 4;

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

class Bus
{
public:
    set<int> GetAvailableSeats(sqlite3* db);
    void BookSeats(sqlite3* db);
};

// TODO: Implement all functions. Responsible for this is Bilol
class Schedule
{
public:
    void GetAvailableSchedules(sqlite3* db);
    void UpdateSchedule();
};

string getCurrentDate();
string chooseDate();
string chooseDeparturePoint();
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

void Passenger::RegisterPassenger(sqlite3* db)
{
    cout << "Enter passenger first name:" << endl;
    getline(cin, fname);

    cout << "Enter passenger last name:" << endl;
    getline(cin, lname);

    cout << "Enter contact information:" << endl;
    getline(cin, contact);

    cout << "Enter email:" << endl;
    getline(cin, email);

    string sql =
        "INSERT INTO Passenger (fname,lname, contact,email) VALUES ('" + fname + "','" + lname + "','" + contact + "','" + email + "');";
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

    if (fname.empty() || lname.empty() || contact.empty() || email.empty())
    {
        cout << "Invalid name or contact information. Registration failed." << std::endl;
        return;
    }

    if (rc != SQLITE_OK)
        cerr << "Error inserting passenger details into database: " << sqlite3_errmsg(db) << std::endl;
    else
        cout << "Passenger registration successful!" << std::endl;
    saveLastName = lname;
}

void Passenger::ViewBookingHistory(sqlite3* db) {}

int Passenger::GetPassengerID(sqlite3* db)
{
    string sql = "SELECT passenger_id FROM Passenger WHERE lname = ?;";

    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        return -1;
    }

    if (sqlite3_bind_text(statement, 1, saveLastName.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        cerr << "Error binding parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(statement);
        return -1;
    }

    int result = sqlite3_step(statement);
    if (result != SQLITE_ROW)
    {
        if (result == SQLITE_DONE)
        {
            cerr << "No matching rows found." << endl;
        }
        else
        {
            cerr << "Error executing statement: " << sqlite3_errmsg(db) << endl;
        }

        sqlite3_finalize(statement);
        return -1;
    }

    int passengerId = sqlite3_column_int(statement, 0);
    sqlite3_finalize(statement);

    return passengerId;
}

// Proceeding payment either by cash or by card
void Payment::ProcessPayment(sqlite3* db)
{
    int PaymentType, rc;
    string sql;
    Passenger PasObj;
    sqlite3_stmt* statement;
    cout << "Choose the method of the payment:\n"
         << "1. By Cash\n"
         << "2. By Card\n"
         << "3. Go Back" << endl;
    cin >> PaymentType;
    cin.ignore();

    switch (PaymentType)
    {
    case 1: // Payment By Cash
        cout << "Enter cash" << endl;

        break;
    case 2: // Payment By Card
        cout << "Card Holder:" << endl;
        getline(cin, cardHolder);
        cout << "Card number:" << endl;
        getline(cin, cardNo);
        cout << "Expericy date:" << endl;
        getline(cin, expiricyDate);
        cout << "CVV:" << endl;
        getline(cin, cvv);

        sql = "INSERT INTO Payment (passenger_id,amount,payment_date,payment_method) VALUES(?,?,?,?);";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr);
        if (rc != SQLITE_OK)
        {
            cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_int(statement, 1, PasObj.GetPassengerID(db));
        if (rc != SQLITE_OK)
        {
            cout << "Failed to bind passenger_id: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_double(statement, 2, amount);
        if (rc != SQLITE_OK)
        {
            cout << "Failed to bind amount: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement, 3, getCurrentDate().c_str(), -1, SQLITE_STATIC);
        if (rc != SQLITE_OK)
        {
            cout << "Failed to bind payment_date: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement, 4, paymethod[0].c_str(), -1, SQLITE_STATIC);
        if (rc != SQLITE_OK)
        {
            cout << "Failed to bind payment_method: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_step(statement);
        if (rc != SQLITE_DONE)
        {
            cout << "Failed to proceed the payment: " << sqlite3_errmsg(db) << endl;
            return;
        }

        sqlite3_finalize(statement);
        cout << "Payment inserted successfully!" << endl;
        break;
    case 3:
        break;
    default:
        cout << "Wrong enter!" << endl;
        break;
    }
}

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

string chooseDate()
{
    time_t currentTime = time(nullptr);
    vector<string> days(7);
    int input;

    cout << "Choose the day you want to travel:" << endl;

    for (int i = 0; i < 7; ++i)
    {
        tm* date = localtime(&currentTime);
        date->tm_mday += i;

        mktime(date);

        int year = date->tm_year + 1900; // Year since 1900
        int month = date->tm_mon + 1;    // Month since January (0-based)
        int day = date->tm_mday;         // Day of the month

        // Format the day as "yyyy-mm-dd"
        std::stringstream ss;
        ss << year << "-" << setfill('0') << std::setw(2) << month << "-" << std::setfill('0') << std::setw(2) << day;
        days[i] = ss.str();

        std::cout << "   " << i + 1 << ") " << days[i] << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Enter your choice (1-7): ";
    std::cin >> input;
    return days[input - 1];
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
    string travelDate = chooseDate();
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

void InsertNewSchedule(sqlite3* db, const string& departurePoint, const string& destinationPoint, const string& travelDate,
    const string& departureTime, const string& destinationTime)
{
    int busId = 0; // Initialize bus_id

    // Choose bus_id based on departure_point
    if (departurePoint == "Girne")
    {
        busId = 1;
    }
    else if (departurePoint == "Lefcosia")
    {
        busId = 2;
    }
    else if (departurePoint == "Famagusta")
    {
        busId = 3;
    }
    if (busId == 0)
    {
        cerr << "Invalid departure point. Unable to determine bus_id." << endl;
        return;
    }

    // Check if the row already exists
    string selectSql = "SELECT COUNT(*) FROM Schedule WHERE departure_point = ? AND destination_point = ? AND travel_date = ? "
                       "AND departure_time = ? AND destination_time = ?;";
    sqlite3_stmt* selectStatement;

    if (sqlite3_prepare_v2(db, selectSql.c_str(), -1, &selectStatement, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing select statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind parameters to the select statement
    if (sqlite3_bind_text(selectStatement, 1, departurePoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(selectStatement, 2, destinationPoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(selectStatement, 3, travelDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(selectStatement, 4, departureTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(selectStatement, 5, destinationTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        cerr << "Error binding select statement parameters: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(selectStatement);
        return;
    }

    int rowCount = 0;
    if (sqlite3_step(selectStatement) == SQLITE_ROW)
    {
        rowCount = sqlite3_column_int(selectStatement, 0);
    }

    sqlite3_finalize(selectStatement);

    if (rowCount > 0)
    {
        cout << "Schedule already exists in the database." << endl;
        return;
    }

    // Insert the new row
    string insertSql = "INSERT INTO Schedule (bus_id, departure_point, destination_point, travel_date, departure_time, destination_time) "
                       "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* insertStatement;

    if (sqlite3_prepare_v2(db, insertSql.c_str(), -1, &insertStatement, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing insert statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind parameters to the insert statement
    if (sqlite3_bind_int(insertStatement, 1, busId) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 2, departurePoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 3, destinationPoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 4, travelDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 5, departureTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 6, destinationTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
    {
        cerr << "Error binding insert statement parameters: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(insertStatement);
        return;
    }

    if (sqlite3_step(insertStatement) != SQLITE_DONE)
    {
        cerr << "Error executing insert statement: " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        cout << "Schedule updated successfully." << endl;
    }

    sqlite3_finalize(insertStatement);
}

void AddSevenNewSchedule(sqlite3* db)
{

    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> nestedMap;
    vector<string> days = comingSevenDays();

    nestedMap["Girne"]["Lefcosia"] = {"07:00", "12:00", "16:00", "20:00"};  // 30mins to one side
    nestedMap["Girne"]["Famagusta"] = {"08:00", "13:00", "17:00", "21:00"}; // 1.5 hours to one side

    nestedMap["Lefcosia"]["Girne"] = {"08:00", "11:00", "14:00", "17:00"};     // 30 mins to one side
    nestedMap["Lefcosia"]["Famagusta"] = {"09:00", "12:00", "15:00", "18:00"}; // 1 hour to one side

    nestedMap["Famagusta"]["Girne"] = {"05:30", "10:30", "15:30", "20:30"};    // 1.5 hours to one side
    nestedMap["Famagusta"]["Lefcosia"] = {"08:30", "13:30", "18:30", "23:30"}; // 1 hour to one side

    for_each(days.begin(), days.end(),
        [&](const string& day)
        {
            // Accessing the nested hashmap and printing the values
            for (const auto& outerCity : nestedMap)
            {
                // getting the departure city
                string departurePoint = outerCity.first;
                for (const auto& innerCity : outerCity.second)
                {
                    // getting the destination city
                    string destinationPoint = innerCity.first;
                    for (const auto& time : innerCity.second)
                    {
                        string departureTime = time;
                        string destinationTime = calculateDestinationTime(departureTime, departurePoint, destinationPoint);
                        InsertNewSchedule(db, departurePoint, destinationPoint, day, departureTime, destinationTime);
                    }
                }
            }
        });
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