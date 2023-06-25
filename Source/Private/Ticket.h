#pragma once

#include "sqlite3.h"
#include <iostream>
#include <chrono>
#include <regex>
#include "common.h"
using namespace std;

class Ticket
{
public:
    void CreateTicket();
    void PrintTicket();
    bool isTicketCancellationValid(sqlite3*, const string&, const string&, int, string);
    bool ModifyChosen(sqlite3* db, int ticketId, const string& userChoice, string userInput);
    void ModifyTicket(sqlite3* db, int ticket_num);

    void CancellationModificationProcess(sqlite3* db);
    bool isEnteredDepDateValid(string enteredTravelDate, string travel_date);
    bool isEnoughTime(const string& departure_time, const string& travel_date);
    bool nameSurnameValidation(sqlite3* db, const std::string& name, const std::string& surname, int ticketNum);
    tm convertToDateStruct(const string& date);

};
