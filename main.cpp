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
    int busId = 0;  // Initialize bus_id
    
    // Choose bus_id based on departure_point
    if (departurePoint == "Girne") {
        busId = 1;
    } else if (departurePoint == "Lefcosia") {
        busId = 2;
    } else if (departurePoint == "Famagusta") {
        busId = 3;
    }
    if (busId == 0) {
        std::cerr << "Invalid departure point. Unable to determine bus_id." << std::endl;
        return;
    }
    
    // Check if the row already exists
    std::string selectSql = "SELECT COUNT(*) FROM Schedule WHERE departure_point = ? AND destination_point = ? AND travel_date = ? "
                            "AND departure_time = ? AND destination_time = ?;";
    sqlite3_stmt* selectStatement;

    if (sqlite3_prepare_v2(db, selectSql.c_str(), -1, &selectStatement, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Bind parameters to the select statement
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
    std::string insertSql = "INSERT INTO Schedule (bus_id, departure_point, destination_point, travel_date, departure_time, destination_time) "
                            "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* insertStatement;

    if (sqlite3_prepare_v2(db, insertSql.c_str(), -1, &insertStatement, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing insert statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Bind parameters to the insert statement
    if (sqlite3_bind_int(insertStatement, 1, busId) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 2, departurePoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 3, destinationPoint.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 4, travelDate.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 5, departureTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(insertStatement, 6, destinationTime.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
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


bool isLettersOnly(const std::string& input) {
    // it will be used to check if the entered names, surnames are letters only or not.
    for (char c : input) {
        if (!std::isalpha(c)) {
            return false;
        }
    }
    return true;
}

bool isValidDateFormat(const std::string& str) {
    // Regular expression pattern for yyyy-mm-dd format
    std::regex pattern(R"(^\d{4}-\d{2}-\d{2}$)");

    return std::regex_match(str, pattern);
}

std::tm convertToDateStruct(const std::string& date) {
    std::istringstream iss(date);
    std::tm result = {};
    char delimiter;

    iss >> result.tm_year >> delimiter >> result.tm_mon >> delimiter >> result.tm_mday;

    result.tm_year -= 1900; // Adjust year to be relative to 1900
    result.tm_mon -= 1;     // Adjust month to be in the range 0-11

    return result;
}

bool isEnoughTime(const std::string& departure_time, const std::string& travel_date) {
    std::string currentTime = getCurrentTime();
    std::string currentDate = getCurrentDate();

    std::istringstream issDeparture(departure_time);
    int departureHour, departureMinute;

    char delimiter;
    issDeparture >> departureHour >> delimiter >> departureMinute;

    int departureTotalMinutes = departureHour * 60 + departureMinute;

    std::istringstream issCurrent(currentTime);
    int currentHour, currentMinute;
    issCurrent >> currentHour >> delimiter >> currentMinute;
    int currentTotalMinutes = currentHour * 60 + currentMinute;

    int timeDifference = departureTotalMinutes - currentTotalMinutes;
    if (timeDifference < 0) {
        timeDifference += 24 * 60; // Add 24 hours to handle next day departures
    }

    std::tm tmTravelDate = convertToDateStruct(travel_date);
    std::tm tmCurrentDate = convertToDateStruct(currentDate);

    if (std::mktime(&tmTravelDate) > std::mktime(&tmCurrentDate)) {

        return true;
    }
    else if(std::mktime(&tmTravelDate) == std::mktime(&tmCurrentDate)){

     if (timeDifference > 180) {

        return true;
        }
    }

    return false;
}
bool nameSurnameValidation(sqlite3* db, const std::string& name, const std::string& surname, int ticketNum) {
    //checking if the given name, surname matches with the data that we have in our db.

    std::string selectSql = "SELECT p.fname, p.lname "
                            "FROM Ticket t "
                            "JOIN Passenger p ON t.passenger_id = p.passenger_id "
                            "WHERE t.ticket_id = ?;";

    sqlite3_stmt* selectStatement;

    if (sqlite3_prepare_v2(db, selectSql.c_str(), -1, &selectStatement, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    if (sqlite3_bind_int(selectStatement, 1, ticketNum) != SQLITE_OK) {
        std::cerr << "Error binding select statement parameters: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(selectStatement);
        return false;
    }

    std::string passengerFirstName;
    std::string passengerLastName;
    if (sqlite3_step(selectStatement) == SQLITE_ROW) {
        passengerFirstName = reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 0));
        passengerLastName = reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 1));
    }

    sqlite3_finalize(selectStatement);

    if (!passengerFirstName.empty() && !passengerLastName.empty()) {
        if (passengerFirstName == name && passengerLastName == surname) {
            std::cout << "Passenger name and surname match." << std::endl;
            return true;
        } else {
            std::cout << "Invalid passenger name or surname." << std::endl;
            return false;
        }
    } else {
        // The ticket does not exist or the provided information is incorrect
        std::cout << "Invalid ticket information." << std::endl;
        return false;
    }
}

bool isEnteredDepDateValid(string enteredTravelDate, string travel_date){
    if (enteredTravelDate != travel_date){
        cout<<"Your entered travel date is wrong, please try again"<<endl;
        return false;
    }
    else{
        return true;
    }
}
bool isTicketCancellationValid(sqlite3* db, const std::string& name, const std::string& surname, int ticketNum, string enteredTravelDate) {
    //to check if the cancellation is valid.
    // it returns true if the ticket exists and isEnoughTime returns true.
    std::string selectSql = "SELECT s.departure_time, s.travel_date "
                            "FROM Ticket t "
                            "JOIN Schedule s ON t.schedule_id = s.schedule_id "
                            "WHERE t.ticket_id = ?;";

    sqlite3_stmt* selectStatement;

    if (sqlite3_prepare_v2(db, selectSql.c_str(), -1, &selectStatement, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    if (sqlite3_bind_int(selectStatement, 1, ticketNum) != SQLITE_OK) {
        std::cerr << "Error binding select statement parameters: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(selectStatement);
        return false;
    }

    std::string departureTime;
    std::string travelDate;
    if (sqlite3_step(selectStatement) == SQLITE_ROW) {
        departureTime = reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 0));
        travelDate = reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 1));
    }

    sqlite3_finalize(selectStatement);

    if(isEnteredDepDateValid(enteredTravelDate, travelDate)){
        if (!departureTime.empty() && !travelDate.empty()) {
            if(nameSurnameValidation(db, name, surname, ticketNum)){

            // The ticket exists, and you have the corresponding departure_time and travel_date
                if(isEnoughTime(departureTime, travelDate)){
                    cout<<"cool you have enought time"<<endl;
                    return true;
                }
                else{
                    cout<<"Unfortunately, you can cancel your ticket at least 3 hours before your departure time."<<endl;
                    return false;
                }
            } else {
                // The ticket does not exist or the provided information is incorrect
                std::cout << "Invalid ticket information." << std::endl;
                return false;
            }
    }
    }
    else{
        return false;
    }

    return false;
}

bool ModifyChosen(sqlite3* db, int ticketId, const std::string& userChoice, string userInput) {
    std::string selectSql = "SELECT passenger_id FROM Ticket WHERE ticket_id = ?;";
    sqlite3_stmt* selectStatement;

    if (sqlite3_prepare_v2(db, selectSql.c_str(), -1, &selectStatement, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    if (sqlite3_bind_int(selectStatement, 1, ticketId) != SQLITE_OK) {
        std::cerr << "Error binding select statement parameters: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(selectStatement);
        return false;
    }

    int passengerId = -1;
    if (sqlite3_step(selectStatement) == SQLITE_ROW) {
        passengerId = sqlite3_column_int(selectStatement, 0);
    }

    sqlite3_finalize(selectStatement);

    if (passengerId != -1) {
        std::string updateSql = "UPDATE Passenger SET "+userChoice+" = ? WHERE passenger_id = ?;";
        sqlite3_stmt* updateStatement;

        if (sqlite3_prepare_v2(db, updateSql.c_str(), -1, &updateStatement, nullptr) != SQLITE_OK) {
            std::cerr << "Error preparing update statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        if (sqlite3_bind_text(updateStatement, 1, userInput.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
            sqlite3_bind_int(updateStatement, 2, passengerId) != SQLITE_OK) {
            std::cerr << "Error binding update statement parameters: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(updateStatement);
            return false;
        }

        if (sqlite3_step(updateStatement) != SQLITE_DONE) {
            std::cerr << "Error updating passenger's "+userChoice+": " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(updateStatement);
            return false;
        }

        sqlite3_finalize(updateStatement);
        return true;
    } else {
        std::cout << "Invalid ticket ID." << std::endl;
        return false;
    }
}


void ModifyTicket(sqlite3* db, int ticket_num){
    int option;
    string userInput;
    cout<<"=============="<<endl;
    cout<<"Ticket Modification"<<endl;
    cout<<"Choose the part you want to modify"<<endl;
    cout<<"1)Name \t 2)Surname \t 3) Contact \t 4) Email"<<endl;
    cin>>option;
    if (option == 1){
        cout<<"Enter new name"<<endl;
        cin>>userInput;
        ModifyChosen(db, ticket_num, "fname", userInput);
    }
    else if(option == 2){
        cout<<"Enter new surname"<<endl;
        cin>>userInput;
        ModifyChosen(db, ticket_num, "lname", userInput);
    }
    else if(option == 3){
        cout<<"Enter new contact"<<endl;
        cin>>userInput;
        ModifyChosen(db, ticket_num, "contact", userInput);
    }
    else if(option == 4){
        while(true){
        cout<<"Enter new email"<<endl;
        cin>>userInput;
        if(isValidEmail(userInput)){
            ModifyChosen(db, ticket_num, "email", userInput);
            break;
        }
        else{
            cout<<"Error, Please try again"<<endl;
        }
        }
    }
    else{
        cout<<"Please, enter valid number"<<endl;
        ModifyTicket(db, ticket_num);
    }
}

void CancellationModificationProcess(sqlite3* db){
    int ticketNum, option;
    std::string name, surname, travelDate;
    bool isValid;

    std::cout << "=======================\n";
    std::cout << " Ticket Cancellation || Ticket Modification\n";
    std::cout << "=======================\n\n";
    std::cout << "Reminder!\nYou can cancel your ticket within 3 hours of your trip.\n\n";

    std::cout << "Please enter the ticket number: ";
    while (!(std::cin >> ticketNum)) {
        std::cout << "Invalid input. The ticket number must be an integer. Please, try again: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true) {
        std::cout << "Please enter your name: ";
        std::getline(std::cin, name);

        if (isLettersOnly(name)) {
            break;
        }

        std::cout << "Invalid input. The name must contain only letters. Please try again." << std::endl;
    }

    while (true) {
        std::cout << "Please enter your surname: ";
        std::getline(std::cin, surname);

        if (isLettersOnly(surname)) {
            break;
        }
        std::cout << "Invalid input. The surname must contain only letters. Please try again." << std::endl;
    }
    while (true) {
        std::cout << "Please enter your travel date in the format (yyyy-mm-dd): ";
        std::getline(std::cin, travelDate);

        // Perform additional validation for the travel date if needed

        if (isValidDateFormat(travelDate)) {
            break;
        }

        std::cout << "Invalid input, please the date should be in the format (yyyy-mm-dd)." << std::endl;
    }
    isValid = isTicketCancellationValid(db, name, surname, ticketNum, travelDate);//checkin if the cancellation/modification valid or not

cout<<"Choose an option.\n 1)Cancel \t 2)Modify"<<endl;
cin>>option;
if(option == 1){
    std::cout << "\n-----------------------\n";
    std::cout << " Confirmation\n";
    std::cout << "-----------------------\n";
    std::cout << "Ticket number: " << ticketNum << std::endl;
    std::cout << "Name: " << name << std::endl;
    std::cout << "Surname: " << surname << std::endl;
    if(isValid){
        cout<<"Cancelled:("<<endl;
    }
    else{
        cout<<"Not cancelled:)";
    }
}
else{
    ModifyTicket(db, ticketNum);
    
}
    
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
    

    // pas.RegisterPassenger(db);
    // pay.ProcessPayment(db);

    // departurePoint = chooseDeparturePoint();
//     destinationPoint = chooseDestinationPoint(departurePoint);
//    GetAvailableSchedules(db,  departurePoint, destinationPoint);
   
    // AddSevenNewSchedule(db); //Adds new schedule for upcoming 7 days.
    // DeleteExpiredSchedules(db); //Deletes all the expired schedules. 
    // CancellationModificationProcess(db);


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
    else
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