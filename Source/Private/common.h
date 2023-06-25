#pragma once

#include <iostream>
#include <regex>

using namespace std;

string getCurrentDate();
string getCurrentTime();
bool isLettersOnly(const std::string& input);
bool isValidDateFormat(const std::string& str);
bool isValidEmail(const string& email);
