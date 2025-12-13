#include "date.h"
#include <iostream>
using namespace std;
// Hàm in ngày tháng năm
void printDate(date d) {
    cout << (d.day < 10 ? "0" : "") << d.day << "/"
         << (d.month < 10 ? "0" : "") << d.month << "/"
         << d.year;
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
