#ifndef INCOME_TRANSACTION_H
#define INCOME_TRANSACTION_H

#include <string>
#include <iostream>
#include "date.h"
// Struct Giao dịch (Data Model)
struct IncomeTransaction {
    date day;
    double amount;
    std::string description;
    int sourceID;
    int walletID;
};

// Class Mảng động
class IncomeArray {
private:
    IncomeTransaction* list; // Con trỏ mảng
    int capacity;            // Sức chứa
    int count;               // Số lượng hiện tại

    void resize();           // Hàm nội bộ để mở rộng bộ nhớ

public:
    IncomeArray(int initialSize = 10);
    ~IncomeArray();

    void addTransaction(date day, double amount, int sourceID, int walletID, std::string desc);
    int getCount();
    IncomeTransaction getAt(int index);
    double getTotalIncome(); // Hàm thống kê

    void saveToFile(const std::string &filename);
    void loadFromFile(const std::string &filename);
};

#endif