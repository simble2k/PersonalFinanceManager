#ifndef DATE_H
#define DATE_H

struct date
{
    int day, month, year;
};
void printDate(date d);
int compareDate(date d1, date d2);
#endif