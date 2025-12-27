#include "Helper.h"

std::string FormatCurrency(long long value) {
    std::string s;

    //Add commas every 3 digits
    do {
        // Get last 3 digits
        long long chunk = value % 1000;
        value /= 1000;

        std::string chunkStr = std::to_string(chunk);
        //If not the last chunk , add leading zeros if needed (case 123,012 -> chunk = 12 -> "0" + chuckStr = "012")
        if (value > 0) {
            while (chunkStr.length() < 3) chunkStr = "0" + chunkStr;
        }
        //Add to front
        s = chunkStr + (s.empty() ? "" : std::string(",") + s);
    } while (value > 0);
    return s + " VND";
}

std::string FormatCurrency(double value) {
    long long rounded = (value >= 0.0) ? static_cast<long long>(value + 0.5)
                                       : static_cast<long long>(value - 0.5);
    return FormatCurrency(rounded);
}

bool IsPositiveMoneyLLong(const std::string& text, long long& outValue) {
    if (text.empty()) return false;
    // Skip leading zeros
    size_t index = 0; // index to traverse the string
    while (index < text.length() && text[index] == '0') {
        index++;
    }
    if (index == text.length()) return false; // String was all zeros

    long long maxValLL = 9223372036854775807LL;

    // Parse the string into a long long integer
    long long acc = 0;
    for (; index < text.length();index++) {
        char ch = text[index];
        // Check if character is a digit and assure no overflow
        if (ch < '0' || ch > '9' || acc > (maxValLL- 10) / 10) return false;
        acc = acc * 10 + (ch - '0');
    }

    outValue = acc;
    return true;
}

static bool IsLeap(int year) {
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

// Expects date in DD/MM/YYYY format
bool ParseDateString(const std::string& text, date& outDate) {
    if (text.size() < 8 || text.size() > 10) return false; 
    // DD/MM/YYYY

    int parts[3] = {0,0,0};
    int partIdx = 0;
    int val = 0;

    // Parse the string
    for (size_t i = 0; i < text.size(); ++i) {
        char ch = text[i];
        if (ch >= '0' && ch <= '9') {
            
            val = val * 10 + (ch - '0');
            if (val > 9999) return false;

        } 
        else if (ch == '/') {
            if (partIdx >= 2) return false;
            parts[partIdx++] = val;
            val = 0;
        } 
        else {
            return false;
        }
    }

    if (partIdx != 2) return false;
    parts[2] = val;

    int d = parts[0];
    int m = parts[1];
    int y = parts[2];
    if (y <= 0 || m < 1 || m > 12 || d < 1) return false;

    int daysInMonth;
    switch (m) {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12: daysInMonth = 31; break;
    case 4: case 6: case 9: case 11: daysInMonth = 30; break;
    case 2: daysInMonth = IsLeap(y) ? 29 : 28; break;
    default: return false;
    }

    if (d > daysInMonth) return false;

    outDate.day = d;
    outDate.month = m;
    outDate.year = y;
    return true;
}

void DrawFormErrorTextIndicator(const Rectangle& addButton, int errorid) {
    if (errorid < 0) return;
    std::string errorText;
    switch (errorid) {
        case 1:
            errorText = "Invalid amount of money! (should be positive integer)";
            break;
        case 2:
            errorText = "Invalid start date! (DD/MM/YYYY)";
            break;
        case 3:
            errorText = "Invalid end date! (DD/MM/YYYY)";
            break;
        case 4:
            errorText = "No wallet selected!";
            break;
        case 5:
            errorText = "No source selected!";
            break;
        case 6:
            errorText = "No category selected!";
            break;
        case 7:
            errorText = "Enter a description!";
            break;
        case 8:
            errorText = "Invalid ID! (should be positive integer)";
        default:
            errorText = "Form contains errors!";
            break;
    }
    int textW = MeasureText(errorText.c_str(), 18);
    DrawText(errorText.c_str(), (int)addButton.x - textW - 20, (int)(addButton.y + 10), 18, RED);
}
