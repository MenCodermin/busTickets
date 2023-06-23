#include "Ticket.h"
using namespace std;

void Ticket::CreateTicket() {}
void Ticket::PrintTicket(){}


bool Ticket::isEnteredDepDateValid(string enteredTravelDate, string travel_date){
    if (enteredTravelDate != travel_date){
        cout<<"Your entered travel date is wrong, please try again"<<endl;
        return false;
    }
    else{
        return true;
    }
}

bool Ticket::nameSurnameValidation(sqlite3* db, const std::string& name, const std::string& surname, int ticketNum) {
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

tm Ticket::convertToDateStruct(const string& date) {
    std::istringstream iss(date);
    std::tm result = {};
    char delimiter;

    iss >> result.tm_year >> delimiter >> result.tm_mon >> delimiter >> result.tm_mday;

    result.tm_year -= 1900; // Adjust year to be relative to 1900
    result.tm_mon -= 1;     // Adjust month to be in the range 0-11

    return result;
}

bool Ticket::isEnoughTime(const std::string& departure_time, const std::string& travel_date) {
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

    tm tmTravelDate = convertToDateStruct(travel_date);
    tm tmCurrentDate = convertToDateStruct(currentDate);

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

bool Ticket::isTicketCancellationValid(sqlite3* db, const string& name, const string& surname, int ticketNum, string enteredTravelDate) {
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

bool Ticket::ModifyChosen(sqlite3* db, int ticketId, const std::string& userChoice, string userInput) {


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

void Ticket::ModifyTicket(sqlite3* db, int ticket_num){
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

void Ticket::CancellationModificationProcess(sqlite3* db){
    int ticketNum, option;
    std::string name, surname, travelDate;
    bool isValid;

    cout << "=======================\n";
    cout << " Ticket Cancellation || Ticket Modification\n";
    cout << "=======================\n\n";
    cout << "Reminder!\nYou can cancel your ticket within 3 hours of your trip.\n\n";

    cout << "Please enter the ticket number: ";
    while (!(cin >> ticketNum)) {
        cout << "Invalid input. The ticket number must be an integer. Please, try again: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (true) {
        std::cout << "Please enter your name: ";
        std::getline(std::cin, name);

        if (isLettersOnly(name)) {
            break;
        }

        cout << "Invalid input. The name must contain only letters. Please try again." << std::endl;
    }

    while (true) {
        cout << "Please enter your surname: ";
        getline(std::cin, surname);

        if (isLettersOnly(surname)) {
            break;
        }
        cout << "Invalid input. The surname must contain only letters. Please try again." << std::endl;
    }
    while (true) {
        cout << "Please enter your travel date in the format (yyyy-mm-dd): ";
        getline(std::cin, travelDate);

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
    cout << "\n-----------------------\n";
    cout << " Confirmation\n";
    cout << "-----------------------\n";
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
