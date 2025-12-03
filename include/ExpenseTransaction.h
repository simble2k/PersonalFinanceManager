#ifndef EXPENSETRANSACTION_H
#define EXPENSETRANSACTION_H
#include <string>
#include <iostream>
#include "date.h"
using namespace std;
// Struct Giao dịch (Data Model)
struct ExpenseTransaction {
    date day;
    double amount;
    string description;
    int categoryID;
    int walletID;
};
// Class Mảng động
class ExpenseArray {
private:
    ExpenseTransaction* list; // Con trỏ mảng
    int capacity;            // Sức chứa
    int count;               // Số lượng hiện tại

    void resize();           // Hàm nội bộ để mở rộng bộ nhớ
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