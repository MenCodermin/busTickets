#include "Source/Private/Schedule.h"

using namespace std;


void Schedule::GetAvailableSchedules(sqlite3* db, const string& departurePoint, const string& destinationPoint) {
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

string Schedule::chooseDate() {
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

vector<string> Schedule::comingSevenDays(){
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
        ss << year << "-" << setfill('0') << setw(2) << month << "-" << setfill('0') << std::setw(2)
           << day;
        days[i] = ss.str();
    }
    return days;
}

string Schedule::calculateDestinationTime(string& departureTime, string& departurePoint, string& destinationPoint) {
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

void Schedule::InsertNewSchedule(sqlite3* db, const string& departurePoint, const string& destinationPoint,
                       const string& travelDate, const string& departureTime, const string& destinationTime)
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


void Schedule::AddSevenNewSchedule(sqlite3* db) {

    unordered_map<string, unordered_map<string, vector<std::string>>> nestedMap;
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

void Schedule::DeleteExpiredSchedules(sqlite3* db) {
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

string Schedule::chooseDeparturePoint() {
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

string Schedule::chooseDestinationPoint(const string& departurePoint) {
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


