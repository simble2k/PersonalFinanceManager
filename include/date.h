#ifndef DATE_H
#define DATE_H

struct date
{
    int day, month, year;
};
void printDate(date d);
date getCurrentDate();
int monthFromLastSession(date last);
int compareDate(date d1, date d2);
bool isLeap(int year);
#endif