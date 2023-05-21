#include <iostream>
#include "lib/sqlite3.h"

using namespace std;

int main() {
    sqlite3* db;
    char* errMsg = nullptr;

    // Open a connection to the database
    int rc = sqlite3_open("bussys.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }
    // Create a table
    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS MyTable (id INT, name TEXT);";
    rc = sqlite3_exec(db, createTableQuery, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return rc;
    }

    // Insert data into the table
    const char* insertDataQuery = "INSERT INTO MyTable (id, name) VALUES (1, 'John'), (2, 'Jane');";
    rc = sqlite3_exec(db, insertDataQuery, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return rc;
    }

    // Retrieve data from the table
    const char* selectDataQuery = "SELECT * FROM MyTable;";
    rc = sqlite3_exec(
        db,
        selectDataQuery,
        [](void* data, int argc, char** argv, char** columnNames) -> int {
            for (int i = 0; i < argc; i++) {
                std::cout << columnNames[i] << ": " << argv[i] << std::endl;
            }
            return 0;
        },
        nullptr,
        &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return rc;
    }
    cin.get();
    // Close the database connection
    sqlite3_close(db);

    return 0;
}






