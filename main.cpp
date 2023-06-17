#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <vector>
#include <utility>
#include "lib/sqlite3.h"
#include <regex>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <chrono>

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
    void GetAvailableSchedules();
private:
    void DeleteExpiredSchedules();
    void AddSevenNewSchedle();
};

// TODO: Implement all functions. Responsible for this is Bilol
class Schedule
{
public:
    void GetAvailableSchedules();
    void AddNewSchedule();
};

string calculateDestinationTime(string& departureTime, string& departurePoint, string& destinationPoint) {
    std::tm tm = {};
    std::istringstream ss(departureTime);
    ss >> std::get_time(&tm, "%H:%M");
    int toOneSide;
    if(departurePoint=="Girne" && destinationPoint == "Lefcosia"){
        toOneSide = 30;
    }
    else if(departurePoint=="Girne" && destinationPoint == "Famagusta"){
         toOneSide = 90;
    }
    else if(departurePoint=="Lefcosia" && destinationPoint == "Girne"){
         toOneSide = 30;
    }
    else if(departurePoint=="Lefcosia" && destinationPoint == "Famagusta"){
         toOneSide = 60;
    }
    else if(departurePoint=="Famagusta" && destinationPoint == "Girne"){
         toOneSide = 90;
    }
    else if(departurePoint=="Famagusta" && destinationPoint == "Lefcosia"){
         toOneSide = 60;
    }

    auto time = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    time += std::chrono::minutes(toOneSide);

    std::time_t destinationTime = std::chrono::system_clock::to_time_t(time);
    std::tm* destinationTm = std::localtime(&destinationTime);

    std::ostringstream oss;
    oss << std::put_time(destinationTm, "%H:%M");

    return oss.str();
}

vector<string> comingSevenDays(){
    time_t currentTime = time(nullptr);
    std::vector<std::string> days(7);
    int input;

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
    }
    return days;
}

void InsertNewSchedule(sqlite3* db, const std::string& departurePoint, const std::string& destinationPoint,
                       const std::string& travelDate, const std::string& departureTime, const std::string& destinationTime)
{
    // Check if the row already exists
    std::string selectSql = "SELECT COUNT(*) FROM Schedule WHERE departure_point = ? AND destination_point = ? AND travel_date = ? "
                            "AND departure_time = ? AND destination_time = ?;";
    sqlite3_stmt* selectStatement;

    if (sqlite3_prepare_v2(db, selectSql.c_str(), -1, &selectStatement, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    if (sqlite3_bind_text(selectStatement, 1, departurePoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(selectStatement, 2, destinationPoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(selectStatement, 3, travelDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(selectStatement, 4, departureTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(selectStatement, 5, destinationTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Error binding select statement parameters: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(selectStatement);
        return;
    }

    int rowCount = 0;
    if (sqlite3_step(selectStatement) == SQLITE_ROW) {
        rowCount = sqlite3_column_int(selectStatement, 0);
    }

    sqlite3_finalize(selectStatement);

    if (rowCount > 0) {
        std::cout << "Schedule already exists in the database." << std::endl;
        return;
    }

    // Insert the new row
    std::string insertSql = "INSERT INTO Schedule (departure_point, destination_point, travel_date, departure_time, destination_time) "
                            "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* insertStatement;

    if (sqlite3_prepare_v2(db, insertSql.c_str(), -1, &insertStatement, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing insert statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    if (sqlite3_bind_text(insertStatement, 1, departurePoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 2, destinationPoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 3, travelDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 4, departureTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 5, destinationTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Error binding insert statement parameters: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(insertStatement);
        return;
    }

    if (sqlite3_step(insertStatement) != SQLITE_DONE) {
        std::cerr << "Error executing insert statement: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Schedule updated successfully." << std::endl;
    }

    sqlite3_finalize(insertStatement);
}


void AddSevenNewSchedule(sqlite3* db) {

    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> nestedMap;
    vector<string>days = comingSevenDays();

    nestedMap["Girne"]["Lefcosia"] = {"07:00", "12:00", "16:00", "20:00"};// 30mins to one side
    nestedMap["Girne"]["Famagusta"] = {"08:00", "13:00", "17:00", "21:00"};//1.5 hours to one side

    nestedMap["Lefcosia"]["Girne"] = {"08:00", "11:00", "14:00", "17:00"}; //30 mins to one side
    nestedMap["Lefcosia"]["Famagusta"] = {"09:00", "12:00", "15:00", "18:00"}; //1 hour to one side

    nestedMap["Famagusta"]["Girne"] = {"05:30", "10:30", "15:30", "20:30"}; //1.5 hours to one side
    nestedMap["Famagusta"]["Lefcosia"] = {"08:30", "13:30", "18:30", "23:30"}; //1 hour to one side

    for_each(days.begin(), days.end(), [&](const string& day) {
    // Accessing the nested hashmap and printing the values
        for (const auto& outerCity : nestedMap) {
            //getting the departure city
            string departurePoint = outerCity.first;
            for (const auto& innerCity : outerCity.second) {
                //getting the destination city
                string destinationPoint = innerCity.first;
                for (const auto& time : innerCity.second) {
                    string departureTime = time;
                    string destinationTime = calculateDestinationTime(departureTime, departurePoint, destinationPoint);
                    InsertNewSchedule(db, departurePoint, destinationPoint, day, departureTime, destinationTime);
                }
            }
        }
    });   
  
}


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

void DeleteExpiredSchedules(sqlite3* db) {
    // Get the current date
    string currentDate = getCurrentDate();

    // Construct the delete query
    std::string deleteSql = "DELETE FROM Schedule WHERE travel_date < ?";
    sqlite3_stmt* deleteStatement;

    if (sqlite3_prepare_v2(db, deleteSql.c_str(), -1, &deleteStatement, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing delete statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    if (sqlite3_bind_text(deleteStatement, 1, currentDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Error binding delete statement parameter: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(deleteStatement);
        return;
    }

    // Execute the delete statement
    if (sqlite3_step(deleteStatement) != SQLITE_DONE) {
        std::cerr << "Error executing delete statement: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Expired schedules deleted successfully." << std::endl;
    }

    sqlite3_finalize(deleteStatement);
}


std::string chooseDate() {
    int input;
    std::cout << "Choose the day you want to travel:" << std::endl;
    vector<string>days = comingSevenDays();

    for (int i=0; i<7; i++){
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
    
    departurePoint = chooseDeparturePoint();
    destinationPoint = chooseDestinationPoint(departurePoint);

    GetAvailableSchedules(db, departurePoint, destinationPoint);
    pas.RegisterPassenger(db);
    pay.ProcessPayment(db);

    // AddSevenNewSchedule(db); //Adds new schedule for upcoming 7 days.
    // DeleteExpiredSchedules(db); //Deletes all the expired schedules. 

    sqlite3_close(db);
    std::cin.get();
    return 0;
}

void Ticket::CreateTicket() {}

void Passenger::RegisterPassenger(sqlite3* db)
{
    std::cout << "Enter passenger first name:" << endl;
    std::getline(std::cin, fname);

    std::cout << "Enter passenger last name:" << endl;
    std::getline(std::cin, lname);

    std::cout << "Enter contact information:" << endl;
    std::getline(std::cin, contact);

bool enteredValidMail = false;
while(!enteredValidMail){
    std::cout << "Enter email:" << endl;
    std::getline(cin, email);
    if (isValidEmail(email)){
        std::cout<<"Your email has been recorded"<<endl;
        enteredValidMail = true;
    }
    {
        std::cout<<"Please, enter a valid email\n"<<endl;
    }
}
    string sql = "INSERT INTO Passenger (fname,lname, contact,email) VALUES ('" + fname + "','" + lname + "','" + contact + "','" + email + "');";
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

    if (fname.empty() || lname.empty() || contact.empty() || email.empty())
    {
        std::cout << "Invalid name or contact information. Registration failed." << std::endl;
        return;
    }

    if (rc != SQLITE_OK)
        cerr << "Error inserting passenger details into database: " << sqlite3_errmsg(db) << std::endl;
    else
        std::cout << "Passenger registration successful!" << std::endl;
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
    std::cout << "Choose the method of the payment:\n"
         << "1. By Cash\n"
         << "2. By Card\n"
         << "3. Go Back" << endl;
    std::cin >> PaymentType;
    std::cin.ignore();

    switch (PaymentType)
    {
    case 1: //Payment By Cash
        std::cout << "Enter cash" << endl;

        break;
    case 2: //Payment By Card
        std::cout << "Card Holder:" << endl;
        std::getline(std::cin, cardHolder);
        std::cout << "Card number:" << endl;
        std::getline(std::cin, cardNo);
        std::cout << "Expericy date:" << endl;
        std::getline(std::cin, expiricyDate);
        std::cout << "CVV:" << endl;
        std::getline(std::cin, cvv);

        sql = "INSERT INTO Payment (passenger_id,amount,payment_date,payment_method) VALUES(?,?,?,?);";
        rc = sqlite3_prepare_v2(db,sql.c_str(), -1, &statement,nullptr);
        if (rc != SQLITE_OK) {
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_int(statement,1,PasObj.GetPassengerID(db));
        if (rc != SQLITE_OK) {
            std::cout << "Failed to bind passenger_id: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_double(statement,2,amount);
        if (rc != SQLITE_OK) {
            std::cout << "Failed to bind amount: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement,3,getCurrentDate().c_str(), -1, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            std::cout << "Failed to bind payment_date: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_bind_text(statement,4,paymethod[0].c_str(),-1,SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            std::cout << "Failed to bind payment_method: " << sqlite3_errmsg(db) << endl;
            return;
        }

        rc = sqlite3_step(statement);
        if (rc != SQLITE_DONE) {
            std::cout << "Failed to proceed the payment: " << sqlite3_errmsg(db) << endl;
            return;
        }

        sqlite3_finalize(statement);
        std::cout<<"Payment inserted successfully!"<<endl;
        break;
    case 3:
        break;
    default:
        std::cout << "Wrong enter!" << endl;
        break;
    }
}