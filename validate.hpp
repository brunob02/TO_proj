#pragma once
#include <chrono>
#include <sstream>
#include <iomanip>

inline bool isAlpha(const std::string& str) {
    return std::all_of(str.begin(), str.end(), [](char c) {
        return std::isalpha(c);
        });
}

inline std::string formatName(std::string str) {
    if (str.empty()) return str;

    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);

    str[0] = std::toupper(str[0]);
    std::transform(str.begin() + 1, str.end(), str.begin() + 1, [](unsigned char c) {
        return std::tolower(c);
        });

    return str;
}

inline bool validateAndParseDate(const std::string& dateStr, std::tm& tm) {
    std::istringstream dateStream(dateStr);
    dateStream >> std::get_time(&tm, "%Y-%m-%d");
    if (dateStream.fail()) {
        int currentYear = std::chrono::system_clock::now().time_since_epoch().count();
        std::string dateWithYear = std::to_string(currentYear) + "-" + dateStr;
        std::istringstream dateStreamWithYear(dateWithYear);
        dateStreamWithYear >> std::get_time(&tm, "%Y-%m-%d");
        if (dateStreamWithYear.fail()) {
            return false;
        }
    }
    return true; 
}

inline bool validateAndParseTime(const std::string& timeStr, std::tm& tm) {
    std::istringstream timeStream(timeStr);
    timeStream >> std::get_time(&tm, "%H:%M:%S");
    if (timeStream.fail()) {
        std::string timeWithMinutesAndSeconds = timeStr + ":00:00";
        std::istringstream timeStreamWithDefaults(timeWithMinutesAndSeconds);
        timeStreamWithDefaults >> std::get_time(&tm, "%H:%M:%S");
        if (timeStreamWithDefaults.fail()) {
            return false;
        }
    }
    return true; 
}

inline void getDateAndTimeFromUser(std::tm& dateTm, std::tm& timeTm) {
    std::string dateStr, timeStr;
    std::cout << "Enter the date (MM-DD or YYYY-MM-DD): ";
    std::cin >> dateStr;
    while (!validateAndParseDate(dateStr, dateTm)) {
        std::cout << "Invalid date format. Please enter a valid date (MM-DD or YYYY-MM-DD): ";
        std::cin >> dateStr;
    }

    std::cout << "Enter the time (HH or HH:MM:SS): ";
    std::cin >> timeStr;
    while (!validateAndParseTime(timeStr, timeTm)) {
        std::cout << "Invalid time format. Please enter a valid time (HH or HH:MM:SS): ";
        std::cin >> timeStr;
    }
}