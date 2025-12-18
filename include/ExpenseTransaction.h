#ifndef EXPENSETRANSACTION_H
#define EXPENSETRANSACTION_H
#include <string>
#include <iostream>
#include "date.h"
struct ExpenseTransaction {
    date day;
    double amount;
    std::string description;
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
    void addTransaction(date day, double amount, int categoryID, int walletID, std::string desc);
    int getCount();
    ExpenseTransaction getAt(int index);
    double getTotalExpense(); // Hàm thống kê
    void saveToFile(const std::string &filename);
    void loadFromFile(const std::string &filename);
};
#endif  