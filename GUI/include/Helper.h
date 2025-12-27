#pragma once

#include <string>
#include "../../include/date.h"
#include "raylib.h"

struct date;

// Common helpers shared by GUI windows.
std::string FormatCurrency(long long value);
std::string FormatCurrency(double value);

// Validation helpers
bool IsPositiveMoneyLLong(const std::string& text, long long& outValue);
bool ParseDateString(const std::string& text, date& outDate);

// UI helpers
void DrawFormErrorTextIndicator(const Rectangle& addButton, int errorid = -1); // 1 for amount ,
                                                                                // 2 for start date,
                                                                                // 3 for end date
                                                                                // 4 for no wallet selected
                                                                                // 5 for no source selected
                                                                                // 6 for no category selected
                                                                                // 7 for empty description
                                                                                // 8 for invalid ID