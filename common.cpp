#include "common.h"
using namespace std;

string getCurrentDate()
{
    time_t currentTime = time(nullptr);
    tm* localTime = localtime(&currentTime);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localTime);

    return string(buffer);

}

string getCurrentTime() {
    
    time_t currentTime = time(nullptr);
    tm* localTime = localtime(&currentTime);

    char buffer[6]; // Buffer to hold the formatted time
    strftime(buffer, sizeof(buffer), "%H:%M", localTime);

    return string(buffer);
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

bool isValidEmail(const string& email) {
    // Regular expression pattern for validating email
    const std::regex pattern(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}\b)");
    
    return std::regex_match(email, pattern);
}