#include "date.h"
#include <iostream>
#include <ctime>
using namespace std;

void printDate(date d) {
    cout << (d.day < 10 ? "0" : "") << d.day << "/"
         << (d.month < 10 ? "0" : "") << d.month << "/"
         << d.year;
}
date getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    date today;
    today.day = ltm->tm_mday;
    today.month = 1 + ltm->tm_mon;
    today.year = 1900 + ltm->tm_year;

    return today;
}
bool isLeap(int year){
    return ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0));
}
int monthFromLastSession(date last) {
    date today = getCurrentDate();

    int yearsDiff = today.year - last.year;
    int monthsDiff = today.month - last.month;
    
    int totalMonths = (yearsDiff * 12) + monthsDiff;

    if (today.day < last.day) {
        totalMonths--;
    }
    return (totalMonths < 0) ? 0 : totalMonths;
}

// Hàm so sánh 2 ngày
int compareDate(date d1, date d2) {
    if (d1.year < d2.year) return -1;
    if (d1.year > d2.year) return 1;
    
    if (d1.month < d2.month) return -1;
    if (d1.month > d2.month) return 1;
    
    if (d1.day < d2.day) return -1;
    if (d1.day > d2.day) return 1;
    
    return 0;
}
