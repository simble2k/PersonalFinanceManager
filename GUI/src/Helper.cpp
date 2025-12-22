#include "Helper.h"

#include <cmath>
#include <cstdio>

std::string FormatCurrency(long long value) {
    bool neg = value < 0;
    unsigned long long uv = neg ? static_cast<unsigned long long>(-value)
                                : static_cast<unsigned long long>(value);
    std::string s;
    do {
        unsigned long long chunk = uv % 1000ULL;
        uv /= 1000ULL;
        char buf[32];
        if (uv > 0) std::snprintf(buf, sizeof(buf), "%03llu", chunk);
        else std::snprintf(buf, sizeof(buf), "%llu", chunk);
        s = std::string(buf) + (s.empty() ? "" : std::string(",") + s);
    } while (uv > 0);
    if (neg) s = "-" + s;
    return s + " VND";
}

std::string FormatCurrency(double value) {
    long long rounded = static_cast<long long>(std::llround(value));
    return FormatCurrency(rounded);
}
