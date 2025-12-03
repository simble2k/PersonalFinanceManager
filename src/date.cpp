#include "date.h"
#include <iostream>
using namespace std;
// Hàm in ngày tháng năm
void printDate(date d) {
    cout << (d.day < 10 ? "0" : "") << d.day << "/"
         << (d.month < 10 ? "0" : "") << d.month << "/"
         << d.year;
}
