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

//Stat time-based
TimeReport getStatTimeBased(date fromDate, date toDate, IncomeArray& incomes, ExpenseArray& expenses) {
    TimeReport report = {0, 0, 0};

    for (int i = 0; i < incomes.getCount(); i++) {
        IncomeTransaction t = incomes.getAt(i);
        if (compareDate(t.day, fromDate) >= 0 && compareDate(t.day, toDate) <= 0) {
            report.totalIncome += t.amount;
        }
    }

    for (int i = 0; i < expenses.getCount(); i++) {
        ExpenseTransaction t = expenses.getAt(i);
        if (compareDate(t.day, fromDate) >= 0 && compareDate(t.day, toDate) <= 0) {
            report.totalExpense += t.amount;
        }
    }

    report.netBalance = report.totalIncome - report.totalExpense;
    return report;
}
//trả về timereport gồm tổng thu, tổng chi, netbalance

//Time & Wallet Based
//Hàm này trả về 1 mảng chứa tổng thu ở tất cả các ví
WalletStatRecord* IncWalletBased(date fromDate, date toDate, IncomeArray& incomes, WalletTable& wallets, int& count) {

    StatRecord* tempRecords = new StatRecord[100]; 
    int countInc = 0;

    for (int i = 0; i < incomes.getCount(); i++) {
        IncomeTransaction t = incomes.getAt(i);
        if (compareDate(t.day, fromDate) >= 0 && compareDate(t.day, toDate) <= 0) {
            updateStatRecord(tempRecords, countInc, t.walletID, t.amount);
        }
    }


    count = countInc; //đây là số lượng ví
    WalletStatRecord* result = new WalletStatRecord[count];

    for (int i = 0; i < count; i++) {
        result[i].walletName = wallets.getWalletName(tempRecords[i].id);
        result[i].amount = tempRecords[i].amount;
    }

    delete[] tempRecords;
    return result;
}//hàm này có 2 thứ cần quan tâm là mảng trả về chứa tổng tiền thu, tên của ví 
//và biến count để đến số lượng ví


//Hàm này trả về 1 mảng chứa tổng chi ở tất cả các ví
WalletStatRecord* ExpWalletBased(date fromDate, date toDate, ExpenseArray& expenses, WalletTable& wallets, int& count) {
    // 1. Mảng tạm
    StatRecord* tempRecords = new StatRecord[100];
    int ExpCount = 0;

    // 2. Quét transaction
    for (int i = 0; i < expenses.getCount(); i++) {
        ExpenseTransaction t = expenses.getAt(i);
        if (compareDate(t.day, fromDate) >= 0 && compareDate(t.day, toDate) <= 0) {
            updateStatRecord(tempRecords, ExpCount, t.walletID, t.amount);
        }
    }

    count = ExpCount;
    WalletStatRecord* result = new WalletStatRecord[count];

    for (int i = 0; i < count; i++) {
        result[i].walletName = wallets.getWalletName(tempRecords[i].id);
        result[i].amount = tempRecords[i].amount;
    }

    delete[] tempRecords;

    return result;
}//hàm này có 2 thứ cần quan tâm là mảng trả về chứa tổng tiền chi, tên của ví 
//và biến count để đến số lượng ví


//Annual Income/Expense Breakdown
bool isYearSelected(int year, int* selectedYears, int count) {
    for (int i = 0; i < count; i++) {
        if (selectedYears[i] == year) {
            return true;
        }
    }
    return false;
}

//giống time report
TimeReport getAnnualOverview(int* selectedYears, int n, IncomeArray& incomes, ExpenseArray& expenses) {
    TimeReport report = {0, 0, 0}; // Khởi tạo bằng 0
    
    for (int i = 0; i < incomes.getCount(); i++) {
        IncomeTransaction t = incomes.getAt(i);
        if (isYearSelected(t.day.year, selectedYears, n)) {
            report.totalIncome += t.amount;
        }
    }

    for (int i = 0; i < expenses.getCount(); i++) {
        ExpenseTransaction t = expenses.getAt(i);
        if (isYearSelected(t.day.year, selectedYears, n)) {
            report.totalExpense += t.amount;
        }
    }

    report.netBalance = report.totalIncome - report.totalExpense;

    return report;
}

//Income and Expense annually Source/Category-Based
BreakdownReport incomeAnnualBreakdown(int* selectedYears, int n, IncomeArray& incomes, IncomeSourceTable& sources) {
    BreakdownReport report;
    report.totalAmount = 0;
    report.count = 0;

    StatRecord* tempRecords = new StatRecord[100]; 
    int incCount = 0;

    for (int i = 0; i < incomes.getCount(); i++) {
        IncomeTransaction t = incomes.getAt(i);
        if (isYearSelected(t.day.year, selectedYears, n)) {
            report.totalAmount += t.amount;
            updateStatRecord(tempRecords, incCount, t.sourceID, t.amount);
        }
    }

    report.items = new BreakdownItem[incCount];
    report.count = incCount;

    for(int i = 0; i < incCount; i++) {
        report.items[i].name = sources.getSourceName(tempRecords[i].id);
        report.items[i].amount = tempRecords[i].amount;
        
        if (report.totalAmount > 0)
            report.items[i].percentage = (report.items[i].amount / report.totalAmount) * 100.0;
        else
            report.items[i].percentage = 0;
    }

    delete[] tempRecords;
    return report; //trả về breakdownreport gồm: tổng thu, số lượng source,
    //mảng breakdownitem chứa record của từng nguồn thu (gồm sourceName, amount, percentage)
}

// Hàm này trả về BreakdownReport (chứa danh sách BreakdownItem)
BreakdownReport expenseAnnualBreakdown(int* selectedYears, int n, ExpenseArray& expenses, CategoryTable& categories) {
    BreakdownReport report;
    report.totalAmount = 0;
    report.count = 0;

    StatRecord* tempRecords = new StatRecord[100]; 
    int expCount = 0;

    for (int i = 0; i < expenses.getCount(); i++) {
        ExpenseTransaction t = expenses.getAt(i);
        
        if (isYearSelected(t.day.year, selectedYears, n)) {
            report.totalAmount += t.amount;
            updateStatRecord(tempRecords, expCount, t.categoryID, t.amount);
        }
    }

    report.items = new BreakdownItem[expCount];
    report.count = expCount;

    for(int i = 0; i < expCount; i++) {
        report.items[i].name = categories.getCategoryName(tempRecords[i].id);
        report.items[i].amount = tempRecords[i].amount;
        
        if (report.totalAmount > 0) {
            report.items[i].percentage = (report.items[i].amount / report.totalAmount) * 100.0;
        } else {
            report.items[i].percentage = 0;
        }
    }

    delete[] tempRecords;
    
    return report;
    //trả về breakdownreport gồm: tổng chi, số lượng category,
    //mảng chứa record của từng nguồn chi (gồm categoryName, amount, percentage)
}

double getWalletBalance(int walletID, IncomeArray& incomes, ExpenseArray& expenses) {
    double totalIncome = 0;
    double totalExpense = 0;

    // 1. Cộng tổng thu của ví này
    for (int i = 0; i < incomes.getCount(); i++) {
        if (incomes.getAt(i).walletID == walletID) {
            totalIncome += incomes.getAt(i).amount;
        }
    }

    // 2. Trừ tổng chi của ví này
    for (int i = 0; i < expenses.getCount(); i++) {
        if (expenses.getAt(i).walletID == walletID) {
            totalExpense += expenses.getAt(i).amount;
        }
    }

    return totalIncome - totalExpense;
}



void viewTransactionHistory(IncomeArray& incomes, ExpenseArray& expenses, 
                            WalletTable& wallets, IncomeSourceTable& sources, CategoryTable& categories) {
    
    cout << "\n================ LICH SU GIAO DICH CHI TIET ================\n";
    
    // 1. IN DANH SÁCH THU NHẬP
    cout << "\n--- THU NHAP (INCOMES) ---\n";
    if (incomes.getCount() == 0) cout << "  (Chua co giao dich nao)\n";
    else {
        cout << left << setw(12) << "Ngay" 
             << left << setw(20) << "Nguon Thu (Name)"  // Yêu cầu: Display Name [cite: 23]
             << left << setw(20) << "Vi (Name)"         // Yêu cầu: Display Name [cite: 25]
             << right << setw(15) << "So Tien" 
             << "   Mo ta\n";
        cout << "--------------------------------------------------------------------------------\n";
        
        for (int i = 0; i < incomes.getCount(); i++) {
            IncomeTransaction t = incomes.getAt(i);
            
            // KỸ THUẬT QUAN TRỌNG: Dùng ID để lấy Tên
            string srcName = sources.getSourceName(t.sourceID);
            string walName = wallets.getWalletName(t.walletID);
            
            printDate(t.day);
            cout << "  " << left << setw(20) << srcName 
                 << left << setw(20) << walName
                 << right << setw(15) << (long long)t.amount 
                 << "   " << t.description << endl;
        }
    }

    // 2. IN DANH SÁCH CHI TIÊU
    cout << "\n--- CHI TIEU (EXPENSES) ---\n";
    if (expenses.getCount() == 0) cout << "  (Chua co giao dich nao)\n";
    else {
        cout << left << setw(12) << "Ngay" 
             << left << setw(20) << "Danh Muc (Name)"   // Yêu cầu: Display Name [cite: 31]
             << left << setw(20) << "Vi (Name)"         // Yêu cầu: Display Name [cite: 33]
             << right << setw(15) << "So Tien" 
             << "   Mo ta\n";
        cout << "--------------------------------------------------------------------------------\n";

        for (int i = 0; i < expenses.getCount(); i++) {
            ExpenseTransaction t = expenses.getAt(i);
            
            // KỸ THUẬT QUAN TRỌNG: Dùng ID để lấy Tên
            string catName = categories.getCategoryName(t.categoryID);
            string walName = wallets.getWalletName(t.walletID);
            
            printDate(t.day);
            cout << "  " << left << setw(20) << catName 
                 << left << setw(20) << walName
                 << right << setw(15) << (long long)t.amount 
                 << "   " << t.description << endl;
        }
    }
    cout << "============================================================\n";
}