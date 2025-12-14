#ifndef EXPENSETRANSACTION_H
#define EXPENSETRANSACTION_H
#include <string>
#include <iostream>
#include "date.h"
using namespace std;
struct ExpenseTransaction {
    date day;
    double amount;
    string description;
    int categoryID;
    int walletID;
};
class ExpenseArray {
private:
    ExpenseTransaction* list;
    int capacity;            
    int count;               

    void resize();           
public:
    ExpenseArray(int initialSize = 10);
    ~ExpenseArray();
    void addTransaction(date day, double amount, int categoryID, int walletID, string desc);
    int getCount();
    ExpenseTransaction getAt(int index);
    double getTotalExpense(); // Hàm thống kê
    void saveToFile(const string &filename);
    void loadFromFile(const string &filename);
};
#endif  