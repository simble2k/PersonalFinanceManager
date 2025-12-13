#include <iostream>
#include <iomanip>
#include "date.h"
#include "IncomeTransaction.h"
#include "ExpenseTransaction.h"
#include "wallet.h"
#include "Category.h"
#include "IncomeSourceTable.h"
#include "statistic.h"

using namespace std;

void updateStatRecord(StatRecord* records, int& count, int id, double amount) {
    for (int i = 0; i < count; i++) {
        if (records[i].id == id) {
            records[i].amount += amount;
            return;
        }
    }
    records[count].id = id;
    records[count].amount = amount;
    count++;
}

void statTimeBased(date fromDate, date toDate, IncomeArray& incomes, ExpenseArray& expenses) {
    double totalInc = 0;
    double totalExp = 0;

    for (int i = 0; i < incomes.getCount(); i++) {
        IncomeTransaction t = incomes.getAt(i);

        if (compareDate(t.day, fromDate) >= 0 && compareDate(t.day, toDate) <= 0) {
            totalInc += t.amount;
        }
    }

    for (int i = 0; i < expenses.getCount(); i++) {
        ExpenseTransaction t = expenses.getAt(i);
        if (compareDate(t.day, fromDate) >= 0 && compareDate(t.day, toDate) <= 0) {
            totalExp += t.amount;
        }
    }

    cout << "\n--- THONG KE THEO THOI GIAN ---\n";
    cout << "Tu ngay: "; printDate(fromDate); cout << " Den ngay: "; printDate(toDate); cout << endl;
    cout << "Tong Thu: " << (size_t)totalInc << endl;
    cout << "Tong Chi: " << (size_t)totalExp << endl;
    cout << "So Du (Net): " << (long long)(totalInc - totalExp) << endl;
}

//Time & Wallet Based
void statWalletBased(date fromDate, date toDate, IncomeArray& incomes, ExpenseArray& expenses, WalletTable& wallets) {
    StatRecord* walletIncs = new StatRecord[100];
    StatRecord* walletExps = new StatRecord[100];
    int countInc = 0;
    int countExp = 0;

    for (int i = 0; i < incomes.getCount(); i++) {
        IncomeTransaction t = incomes.getAt(i);
        if (compareDate(t.day, fromDate) >= 0 && compareDate(t.day, toDate) <= 0) {
            updateStatRecord(walletIncs, countInc, t.walletID, t.amount);
        }
    }

    for (int i = 0; i < expenses.getCount(); i++) {
        ExpenseTransaction t = expenses.getAt(i);
        if (compareDate(t.day, fromDate) >= 0 && compareDate(t.day, toDate) <= 0) {
            updateStatRecord(walletExps, countExp, t.walletID, t.amount);
        }
    }



    cout << "\n--- CHI TIET THEO VI (WALLET) ---\n";
    cout << left << setw(25) << "Ten Vi" 
         << right << setw(15) << "Thu Vao" 
         << right << setw(15) << "Chi Ra" << endl;
    
    // In các ví có Thu
    for(int i=0; i<countInc; i++) {
         string name = wallets.getWalletName(walletIncs[i].id);
         cout << left << setw(25) << name 
              << right << setw(15) << (size_t)walletIncs[i].amount 
              << right << setw(15) << "-" << endl;
    }
    // In các ví có Chi 
    for(int i=0; i<countExp; i++) {
         string name = wallets.getWalletName(walletExps[i].id);
         cout << left << setw(25) << name 
              << right << setw(15) << "-" 
              << right << setw(15) << (size_t)walletExps[i].amount << endl;
    }

    delete[] walletIncs;
    delete[] walletExps;
}

//Annual Income/Expense Breakdown
bool isYearSelected(int year, int* selectedYears, int count) {
    for (int i = 0; i < count; i++) {
        if (selectedYears[i] == year) {
            return true;
        }
    }
    return false;
}

void statAnnualOverview(IncomeArray& incomes, ExpenseArray& expenses) {
    int n;
    cout << "\n--- TONG QUAN TAI CHINH THEO NAM ---\n";
    cout << "Ban muon xem bao cao cho bao nhieu nam? (Nhap so luong): ";
    cin >> n;

    if (n <= 0) {
        cout << "So luong nam khong hop le!\n";
        return;
    }

    int* selectedYears = new int[n];
    cout << "Nhap cac nam can xem (cach nhau boi dau cach): ";
    for (int i = 0; i < n; i++) {
        cin >> selectedYears[i];
    }

    double totalInc = 0;
    double totalExp = 0;

    //Income
    for (int i = 0; i < incomes.getCount(); i++) {
        IncomeTransaction t = incomes.getAt(i);

        if (isYearSelected(t.day.year, selectedYears, n)) {
            totalInc += t.amount;
        }
    }

    //Expense
    for (int i = 0; i < expenses.getCount(); i++) {
        ExpenseTransaction t = expenses.getAt(i);

        if (isYearSelected(t.day.year, selectedYears, n)) {
            totalExp += t.amount;
        }
    }

    cout << "\n--------------------------------------------\n";
    cout << "BAO CAO TONG HOP NAM: ";
    for (int i = 0; i < n; i++) cout << selectedYears[i] << (i < n-1 ? ", " : "");
    cout << endl;
    cout << "--------------------------------------------\n";
    cout << left << setw(20) << "TONG THU (Income): "  << right << setw(15) << (size_t)totalInc << endl;
    cout << left << setw(20) << "TONG CHI (Expense): " << right << setw(15) << (size_t)totalExp << endl;
    cout << "--------------------------------------------\n";
    
    double netBalance = totalInc - totalExp;
    cout << left << setw(20) << "SO DU (Net Balance): " << right << setw(15) << (long long)netBalance << endl;
    cout << "============================================\n";

    delete[] selectedYears;
}

//Income and Expense annually Source/Category-Based
void incomeAnnualBreakdown(IncomeArray& incomes, IncomeSourceTable& sources) {
    int n;
    cout << "Ban muon xem bao cao trong bao nhieu nam: ";
    cin >> n;
    
    int* selectedYear = new int[n];
    cout << "Nhap cac nam can xem: ";
    for (int i = 0; i< n;i++) {
        cin >> selectedYear[i];
    }

    int targetID;
    cout << "Ban muon xem bao cao tu nguon thu nao (nhap ID): ";
    cin >> targetID;


    double totalIncome = 0.0;

    for (int i =0;i < incomes.getCount(); i++) {
        IncomeTransaction t = incomes.getAt(i);
        
        if (isYearSelected(t.day.year, selectedYear, n) && (t.sourceID == targetID)) {
            totalIncome += t.amount;
        }
    }

    string sourceName = sources.getSourceName(targetID);
    cout << "\n--------------------------------------------\n";
    cout << "BAO CAO TONG HOP NAM: ";
    for (int i = 0; i < n; i++) cout << selectedYear[i] << (i < n-1 ? ", " : "");
    cout << endl;
    cout << "Nguon thu: " << sourceName << endl;
    cout << "Tong thu: " << totalIncome << endl;
    cout << "============================================\n";
    delete[] selectedYear;
}

void expenseAnnualBreakdown(ExpenseArray& expenses, CategoryTable& categories) {
    int n;
    cout << "Ban muon xem bao cao trong bao nhieu nam: ";
    cin >> n;
    
    int* selectedYear = new int[n];
    cout << "Nhap cac nam can xem: ";
    for (int i =0;i < n;i++) {
        cin >> selectedYear[i];
    }

    int targetID;
    cout << "Ban muon xem bao cao theo danh muc nao (nhap ID): ";
    cin >> targetID;
    double totalExpense = 0.0;
    for (int i =0;i < expenses.getCount(); i++) {
        ExpenseTransaction t = expenses.getAt(i);
        
        if (isYearSelected(t.day.year, selectedYear, n) && (t.categoryID == targetID)) {
            totalExpense += t.amount;
        }
    }
    string categoryName = categories.getCategoryName(targetID);
    cout << "\n--------------------------------------------\n";
    cout << "BAO CAO TONG HOP NAM: ";
    for (int i = 0; i < n; i++) cout << selectedYear[i] << (i < n-1 ? ", " : "");
    cout << endl;
    cout << "Danh muc: " << categoryName << endl;
    cout << "Tong chi: " << totalExpense << endl;
    cout << "============================================\n";
    delete[] selectedYear;
}