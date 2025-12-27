#include <iostream>
#include <iomanip>
#include "date.h"
#include "IncomeTransaction.h"
#include "ExpenseTransaction.h"
#include "wallet.h"
#include "Category.h"
#include "IncomeSourceTable.h"
#include "statistic.h"

// Helper to update or add a StatRecord in an array
void updateStatRecord(StatRecord* records, int& count, int id, double amount) {

    // Check if record exists
    for (int i = 0; i < count; i++) {
        // If found, update the amount
        if (records[i].id == id) {
            records[i].amount += amount;
            return;
        }
    }

    // Not found, add new record
    records[count].id = id;
    records[count].amount = amount;
    count++;
}

//Time Based and Overall
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
    cout << "Tong Thu: " << (long long)totalInc << endl;
    cout << "Tong Chi: " << (long long)totalExp << endl;
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

    // In báo cáo
    cout << "\n--- CHI TIET THEO VI (WALLET) ---\n  ";
    cout << left << setw(25) << "Ten Vi" 
         << right << setw(15) << "Thu Vao" 
         << right << setw(15) << "Chi Ra" << endl;
    
    // In các ví có Thu
    for(int i=0; i<countInc; i++) {
         string name = wallets.getWalletName(walletIncs[i].id);
         cout << left << setw(25) << name 
              << right << setw(15) << (long long)walletIncs[i].amount 
              << right << setw(15) << "-" << endl;
    }
    // In các ví có Chi 
    for(int i=0; i<countExp; i++) {
         string name = wallets.getWalletName(walletExps[i].id);
         cout << left << setw(25) << name 
              << right << setw(15) << "-" 
              << right << setw(15) << (long long)walletExps[i].amount << endl;
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

//Overall Annual Overview
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
    cout << left << setw(20) << "TONG THU (Income): "  << right << setw(15) << (long long)totalInc << endl;
    cout << left << setw(20) << "TONG CHI (Expense): " << right << setw(15) << (long long)totalExp << endl;
    cout << "--------------------------------------------\n";
    
    double netBalance = totalInc - totalExp;
    cout << left << setw(20) << "SO DU (Net Balance): " << right << setw(15) << (long long)netBalance << endl;
    cout << "============================================\n";

    delete[] selectedYears;
}

//Income and Expense annually Source/Category-Based
void incomeAnnualBreakdown(IncomeArray& incomes, IncomeSourceTable& sources) {
    int n;
    cout << "\n--- PHAN TICH NGUON THU THEO NAM ---\n";
    cout << "Ban muon xem bao cao cho bao nhieu nam? "; cin >> n;
    
    int* selectedYears = new int[n];
    cout << "Nhap cac nam (cach nhau boi dau cach): ";
    for (int i = 0; i < n; i++) cin >> selectedYears[i];

    // Dùng mảng tạm để thống kê (Max 100 nguồn thu - đủ cho project)
    StatRecord* records = new StatRecord[100]; 
    int count = 0;
    double totalAll = 0;

    // Duyệt qua tất cả giao dịch
    for (int i = 0; i < incomes.getCount(); i++) {
        IncomeTransaction t = incomes.getAt(i);
        
        // Nếu giao dịch thuộc năm đã chọn
        if (isYearSelected(t.day.year, selectedYears, n)) {
            totalAll += t.amount;
            updateStatRecord(records, count, t.sourceID, t.amount);
        }
    }

    // Hiển thị báo cáo
    cout << "\n------------------------------------------------------------\n";
    cout << "BAO CAO PHAN TICH NGUON THU (Nam: ";
    for (int i = 0; i < n; i++) cout << selectedYears[i] << (i < n-1 ? ", " : "");
    cout << ")\n";
    cout << "------------------------------------------------------------\n";
    cout << left << setw(25) << "Nguon Thu" 
         << right << setw(15) << "So Tien" 
         << right << setw(10) << "Ti Le" << endl;
    cout << "------------------------------------------------------------\n";

    if (totalAll == 0) {
        cout << "  (Khong co du lieu thu nhap trong nam nay)\n";
    } else {
        for (int i = 0; i < count; i++) {
            string name = sources.getSourceName(records[i].id);
            double percent = (records[i].amount / totalAll) * 100.0;
            
            cout << left << setw(25) << name 
                 << right << setw(15) << (long long)records[i].amount 
                 << right << setw(9) << fixed << setprecision(1) << percent << "%" << endl;
        }
        cout << "------------------------------------------------------------\n";
        cout << left << setw(25) << "TONG CONG:" 
             << right << setw(15) << (long long)totalAll << endl;
    }
    cout << "============================================================\n";

    delete[] selectedYears;
    delete[] records;
}

//Expense annually Category-Based
void expenseAnnualBreakdown(ExpenseArray& expenses, CategoryTable& categories) {
    int n;
    cout << "\n--- PHAN TICH CHI TIEU THEO DANH MUC ---\n";
    cout << "Ban muon xem bao cao cho bao nhieu nam? "; cin >> n;
    
    int* selectedYears = new int[n];
    cout << "Nhap cac nam (cach nhau boi dau cach): ";
    for (int i = 0; i < n; i++) cin >> selectedYears[i];

    StatRecord* records = new StatRecord[100];
    int count = 0;
    double totalAll = 0;

    for (int i = 0; i < expenses.getCount(); i++) {
        ExpenseTransaction t = expenses.getAt(i);
        
        if (isYearSelected(t.day.year, selectedYears, n)) {
            totalAll += t.amount;
            updateStatRecord(records, count, t.categoryID, t.amount);
        }
    }

    cout << "\n------------------------------------------------------------\n";
    cout << "BAO CAO PHAN TICH CHI TIEU (Nam: ";
    for (int i = 0; i < n; i++) cout << selectedYears[i] << (i < n-1 ? ", " : "");
    cout << ")\n";
    cout << "------------------------------------------------------------\n";
    cout << left << setw(25) << "Danh Muc Chi" 
         << right << setw(15) << "So Tien" 
         << right << setw(10) << "Ti Le" << endl;
    cout << "------------------------------------------------------------\n";

    if (totalAll == 0) {
        cout << "  (Khong co du lieu chi tieu trong nam nay)\n";
    } else {
        for (int i = 0; i < count; i++) {
            string name = categories.getCategoryName(records[i].id);
            double percent = (records[i].amount / totalAll) * 100.0;
            
            cout << left << setw(25) << name 
                 << right << setw(15) << (long long)records[i].amount 
                 << right << setw(9) << fixed << setprecision(1) << percent << "%" << endl;
        }
        cout << "------------------------------------------------------------\n";
        cout << left << setw(25) << "TONG CONG:" 
             << right << setw(15) << (long long)totalAll << endl;
    }
    cout << "============================================================\n";

    delete[] selectedYears;
    delete[] records;
}

//Main Statistic Menu
void statisticMenu(IncomeArray& incomes, ExpenseArray& expenses,
                   WalletTable& wallets, IncomeSourceTable& sources, CategoryTable& categories) {
    
    // Menu loop
    int choice;

    do {
        cout << "\n=== STATISTICS & REPORTING ===\n";
        cout << "1. Xem Lich su giao dich chi tiet (Transaction Log)\n"; // <--- MỚI
        cout << "2. Thong ke theo khoang thoi gian (Time-based)\n";
        cout << "3. Thong ke chi tiet theo Vi (Wallet-based)\n";
        cout << "4. Tong quan Thu/Chi nhieu nam (Overview)\n";
        cout << "5. Phan tich Nguon Thu (Income Breakdown)\n";
        cout << "6. Phan tich Danh Muc Chi (Expense Breakdown)\n";
        cout << "0. Quay lai\n";
        cout << "Chon chuc nang: ";
        cin >> choice;

        switch(choice) {
            case 1:
                viewTransactionHistory(incomes, expenses, wallets, sources, categories);
                break;
            case 2:
                { date d1, d2; /*...*/ statTimeBased(d1, d2, incomes, expenses); }
                break;
            case 3:
                { date d1, d2; /*...*/ statWalletBased(d1, d2, incomes, expenses, wallets); }
                break;
            case 4: statAnnualOverview(incomes, expenses); break;
            case 5: incomeAnnualBreakdown(incomes, sources); break;
            case 6: expenseAnnualBreakdown(expenses, categories); break;
        }
    } while (choice != 0);
}

//Calculate Wallet Balance
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

//View Detailed Transaction History
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