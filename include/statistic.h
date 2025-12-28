#ifndef STATISTIC_H
#define STATISTIC_H
#include <iostream>
#include <iomanip>
#include <string>
#include "date.h"
#include "IncomeTransaction.h"
#include "ExpenseTransaction.h"
#include "wallet.h"
#include "Category.h"
#include "IncomeSourceTable.h"

struct TimeReport {
    double totalIncome;
    double totalExpense;
    double netBalance;
};

struct BreakdownItem {
    std::string name;       
    double amount;     
    double percentage; 
};

struct BreakdownReport {
    double totalAmount;
    int count;             // Số lượng phần tử trong mảng items
    BreakdownItem* items;  // Con trỏ quản lý mảng động
};

struct WalletStatRecord {
    std::string walletName;
    double amount;
};

struct StatRecord {
    int id;         // ID của Wallet, Category, hoặc Source
    double amount;  // Tổng tiền
};

void updateStatRecord(StatRecord* records, int& count, int id, double amount);
TimeReport getStatTimeBased(date fromDate, date toDate, IncomeArray& incomes, ExpenseArray& expenses);
WalletStatRecord* IncWalletBased(date fromDate, date toDate, IncomeArray& incomes, WalletTable& wallets, int& count);
WalletStatRecord* ExpWalletBased(date fromDate, date toDate, ExpenseArray& expenses, WalletTable& wallets, int& count);
TimeReport getAnnualOverview(int* selectedYears, int n, IncomeArray& incomes, ExpenseArray& expenses);
bool isYearSelected(int year, int* selectedYears, int count);
BreakdownReport incomeAnnualBreakdown(int* selectedYears, int n, IncomeArray& incomes, IncomeSourceTable& sources);
BreakdownReport expenseAnnualBreakdown(int* selectedYears, int n, ExpenseArray& expenses, CategoryTable& categories);
double getWalletBalance(int walletID, IncomeArray& incomes, ExpenseArray& expenses);
void viewTransactionHistory(IncomeArray& incomes, ExpenseArray& expenses, 
                            WalletTable& wallets, IncomeSourceTable& sources, CategoryTable& categories);
#endif