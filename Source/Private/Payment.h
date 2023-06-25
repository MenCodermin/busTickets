#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>

class Payment
{
public:
    void ProcessPayment(sqlite3* db);

private:
    double amount;
    std::string cardNo;
    std::string cardHolder;
    std::string expiricyDate;
    std::string cvv;
    std::vector<std::string> paymethod{"card", "cash"};
};