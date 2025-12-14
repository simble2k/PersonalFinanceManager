#ifndef DATE_H
#define DATE_H

struct date
{
    int day, month, year;
};
void printDate(date d);
date getCurrentDate();
int monthFromLastSession(date last);
#endif