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
#include "Source/Private/Passenger.h"
#include "Source/Private/Payment.h"
#include "Source/Private/Bus.h"
#include "Source/Private/Ticket.h"
#include "Source/Private/Schedule.h"
#include "Source/Private/common.h"

using namespace std;

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

    cin.get();

    sqlite3_close(db);

    return 0;
}
