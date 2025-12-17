#include <iostream>
#include <iomanip>
#include "date.h"
#include "IncomeTransaction.h"
#include "ExpenseTransaction.h"
#include "wallet.h"
#include "Category.h"
#include "IncomeSourceTable.h"

//Dùng để lưu tạm chi/tiêu theo wallet/category/source
#pragma once
struct StatRecord {
    int id;         // ID của Wallet, Category, hoặc Source
    double amount;  // Tổng tiền
};

void updateStatRecord(StatRecord* records, int& count, int id, double amount);
void statTimeBased(date fromDate, date toDate, IncomeArray& incomes, ExpenseArray& expenses);
void statWalletBased(date fromDate, date toDate, IncomeArray& incomes, ExpenseArray& expenses, WalletTable& wallets);
void statAnnualOverview(IncomeArray& incomes, ExpenseArray& expenses);
bool isYearSelected(int year, int* selectedYears, int count);
void incomeAnnualBreakdown(IncomeArray& incomes, IncomeSourceTable& sources);
void expenseAnnualBreakdown(ExpenseArray& expenses, CategoryTable& categories);
void statisticMenu(IncomeArray& incomes, ExpenseArray& expenses, 
                   WalletTable& wallets, IncomeSourceTable& sources, CategoryTable& categories);
double getWalletBalance(int walletID, IncomeArray& incomes, ExpenseArray& expenses);
void viewTransactionHistory(IncomeArray& incomes, ExpenseArray& expenses, 
                            WalletTable& wallets, IncomeSourceTable& sources, CategoryTable& categories);