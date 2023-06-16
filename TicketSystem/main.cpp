#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <vector>
#include <utility>
#include "lib/sqlite3.h"
#include <regex>
#include <iomanip> 

using namespace std;


string saveLastName = "";

// TODO: Implement all functions. Responsible for this is Nazar
class Payment
{
public:
    void ProcessPayment(sqlite3* db);
    void ValidatePayment();

private:
    double amount;
    string cardNo;
    string cardHolder;
    string expiricyDate;
    string cvv;
    vector<string> paymethod {"card","cash"};
};

// TODO: Implement all functions. Responsible for this is Nazar
class Ticket
{
public:
    void CreateTicket();
    void PrintTicket();
    void CancelTicket();
};

// TODO: Implement all functions. Responsible for this is Nazar
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
    void GetAvailableSeats();
    void BookSeats();
};

// TODO: Implement all functions. Responsible for this is Bilol
class Route
{
public:
    void GetRouteDetails();
};

// TODO: Implement all functions. Responsible for this is Bilol
class Schedule
{
public:
    void GetAvailableSchedules();
    void UpdateSchedule();
};

string getCurrentDate()
{
    time_t currentTime = time(nullptr);
    tm* localTime = localtime(&currentTime);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localTime);

    return string(buffer);
}

string getCurrentTime() {
    
    time_t currentTime = std::time(nullptr);
    tm* localTime = std::localtime(&currentTime);

    char buffer[6]; // Buffer to hold the formatted time
    std::strftime(buffer, sizeof(buffer), "%H:%M", localTime);

    return std::string(buffer);
}

std::string chooseDate() {
    time_t currentTime = time(nullptr);
    std::vector<std::string> days(7);
    int input;

    std::cout << "Choose the day you want to travel:" << std::endl;

    for (int i = 0; i < 7; ++i) {
        tm* date = localtime(&currentTime);
        date->tm_mday += i;

        mktime(date);

        int year = date->tm_year + 1900; // Year since 1900
        int month = date->tm_mon + 1;   // Month since January (0-based)
        int day = date->tm_mday;        // Day of the month

        // Format the day as "yyyy-mm-dd"
        std::stringstream ss;
        ss << year << "-" << std::setfill('0') << std::setw(2) << month << "-" << std::setfill('0') << std::setw(2)
           << day;
        days[i] = ss.str();

        std::cout << "   " << i + 1 << ") " << days[i] << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Enter your choice (1-7): ";
    std::cin >> input;
    return days[input - 1];
}


std::string chooseDeparturePoint() {
    std::string cities[] = {"Girne", "Lefcosia", "Famagusta"};
    int cityIndex;

    std::cout << "============ Departure Points ============" << std::endl;
    std::cout << "Choose your departure point:" << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "   " << i + 1 << ") " << cities[i] << std::endl;
    }
    std::cout << "==========================================" << std::endl;

    std::cin >> cityIndex;
    if (cityIndex >= 1 && cityIndex <= 3) {
        return cities[cityIndex - 1];
    } else {
        std::cout << "Invalid city index selected." << std::endl;
        return ""; // Return an empty string to indicate an error
    }
}

std::string chooseDestinationPoint(const std::string& departurePoint) {
    std::vector<std::string> cities = {"Girne", "Lefcosia", "Famagusta"};

    // Find the departure point in the vector
    auto targetIter = std::find(cities.begin(), cities.end(), departurePoint);
    if (targetIter != cities.end()) {
        // Erase the departure point from the vector
        cities.erase(targetIter);
    } else {
        std::cout << "Departure point not found in the cities list." << std::endl;
        return ""; // Return an empty string to indicate an error
    }

    // Display an attractive menu
    std::cout << "============ Destination Points ============" << std::endl;
    std::cout << "Choose your destination point:" << std::endl;
    for (size_t i = 0; i < cities.size(); i++) {
        std::cout << "   " << i + 1 << ") " << cities[i] << std::endl;
    }
    std::cout << "============================================" << std::endl;

    int cityIndex;
    std::cin >> cityIndex;
    if (cityIndex >= 1 && cityIndex <= static_cast<int>(cities.size())) {
        return cities[cityIndex - 1];
    } else {
        std::cout << "Invalid city index selected." << std::endl;
        return ""; // Return an empty string to indicate an error
    }
}

void GetAvailableSchedules(sqlite3* db, const std::string& departurePoint, const std::string& destinationPoint) {
    std::vector<std::string> depTimeOptions;
    int depTimeIndex;
    std::string travelDate = chooseDate();
    std::string sql = "SELECT departure_time FROM Schedule WHERE departure_point = ? AND destination_point = ? AND travel_date = ?;";
    sqlite3_stmt* statement;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    if (sqlite3_bind_text(statement, 1, departurePoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(statement, 2, destinationPoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(statement, 3, travelDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Error binding parameters: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(statement);
        return;
    }

    while (sqlite3_step(statement) == SQLITE_ROW) {
        const unsigned char* departureTime = sqlite3_column_text(statement, 0);
        std::string strDepartureTime(reinterpret_cast<const char*>(departureTime));

        if (travelDate != getCurrentDate() || strDepartureTime > getCurrentTime()) {
            depTimeOptions.push_back(strDepartureTime);
        }
    }

    sqlite3_finalize(statement);

    if (!depTimeOptions.empty()) {
        std::cout << "============ Available Departure Times ============" << std::endl;
        std::cout << "Choose the time you want to travel:" << std::endl;
        for (size_t i = 0; i < depTimeOptions.size(); i++) {
            std::cout << "   " << i + 1 << ") " << depTimeOptions[i] << std::endl;
        }
        std::cout << "==================================================" << std::endl;
    } else {
        std::cout << "There are no available buses for the chosen time. Please choose another day." << std::endl;
        GetAvailableSchedules(db, departurePoint, destinationPoint);
        return;
    }

    std::cin >> depTimeIndex;

    if (depTimeIndex >= 1 && depTimeIndex <= static_cast<int>(depTimeOptions.size())) {
        std::string chosenDepartureTime = depTimeOptions[depTimeIndex - 1];
        std::cout << "You have chosen the departure time: " << chosenDepartureTime << std::endl;
    } else {
        std::cout << "Invalid departure time index selected." << std::endl;
    }
}


bool isValidEmail(const string& email);

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
    // cin.get();
    departurePoint = chooseDeparturePoint();
    destinationPoint = chooseDestinationPoint(departurePoint);

    GetAvailableSchedules(db, departurePoint, destinationPoint);
    sqlite3_close(db);
    std::cin.get();
    return 0;
}

void Ticket::CreateTicket() {}

void Passenger::RegisterPassenger(sqlite3* db)
{
    cout << "Enter passenger first name:" << endl;
    std::getline(std::cin, fname);

    cout << "Enter passenger last name:" << endl;
    std::getline(std::cin, lname);

    cout << "Enter contact information:" << endl;
    std::getline(std::cin, contact);

bool enteredValidMail = false;
while(!enteredValidMail){
    cout << "Enter email:" << endl;
    std::getline(std::cin, email);
    if (isValidEmail(email)){
        cout<<"Your email has been recorded"<<endl;
        enteredValidMail = true;
    }
    {
        cout<<"Please, enter a valid email\n"<<endl;
    }
}
    string sql = "INSERT INTO Passenger (fname,lname, contact,email) VALUES ('" + fname + "','" + lname + "','" + contact + "','" + email + "');";
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

bool isValidEmail(const string& email) {
    // Regular expression pattern for validating email
    const std::regex pattern(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}\b)");
    
    return std::regex_match(email, pattern);
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
    int PaymentType,rc;
    string sql;
    Passenger PasObj;
    sqlite3_stmt* statement;
    cout << "Choose the method of the payment:\n"
         << "1. By Cash\n"
         << "2. By Card\n"
         << "3. Go Back" << endl;
    std::cin >> PaymentType;
    std::cin.ignore();

    switch (PaymentType)
    {
    case 1: //Payment By Cash
        cout << "Enter cash" << endl;

        break;
    case 2: //Payment By Card
        cout << "Card Holder:" << endl;
        std::getline(std::cin, cardHolder);
        cout << "Card number:" << endl;
        std::getline(std::cin, cardNo);
        cout << "Expericy date:" << endl;
        std::getline(std::cin, expiricyDate);
        cout << "CVV:" << endl;
        std::getline(std::cin, cvv);

        sql = "INSERT INTO Payment (passenger_id,amount,payment_date,payment_method) VALUES(?,?,?,?);";
        rc = sqlite3_prepare_v2(db,sql.c_str(), -1, &statement,nullptr);
        if (rc != SQLITE_OK) {
            cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_int(statement,1,PasObj.GetPassengerID(db));
        if (rc != SQLITE_OK) {
            cout << "Failed to bind passenger_id: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_double(statement,2,amount);
        if (rc != SQLITE_OK) {
            cout << "Failed to bind amount: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement,3,getCurrentDate().c_str(), -1, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            cout << "Failed to bind payment_date: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement,4,paymethod[0].c_str(),-1,SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            cout << "Failed to bind payment_method: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_step(statement);
        if (rc != SQLITE_DONE) {
            cout << "Failed to proceed the payment: " << sqlite3_errmsg(db) << endl;
            return;
        }

        sqlite3_finalize(statement);
        cout<<"Payment inserted successfully!"<<endl;
        break;
    case 3:
        break;
    default:
        cout << "Wrong enter!" << endl;
        break;
    }
}