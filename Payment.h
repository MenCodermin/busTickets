#include <iostream>
#include "lib/sqlite3.h"
#include <vector>

using namespace std;
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