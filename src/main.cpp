#include <iostream>
#include <iomanip>
#include "date.h"
#include "wallet.h"
#include "Category.h"
#include "IncomeSourceTable.h"
#include "IncomeTransaction.h"
#include "ExpenseTransaction.h"

using namespace std;

// Hàm in báo cáo tổng hợp (Kết nối tất cả các thành phần)
void printReport(IncomeArray& incomes, ExpenseArray& expenses, 
                 WalletTable& wallets, IncomeSourceTable& sources, CategoryTable& categories) {
    
    cout << "\n================ BAO CAO TAI CHINH ================\n";
    
    // 1. Hiển thị danh sách thu nhập
    cout << "\n--- DANH SACH THU NHAP ---\n";
    cout << left << setw(12) << "Ngay" 
         << left << setw(20) << "Nguon Thu" 
         << left << setw(20) << "Vi Tien" 
         << right << setw(15) << "So Tien" 
         << "   Mo ta" << endl;
    
    for (int i = 0; i < incomes.getCount(); i++) {
        IncomeTransaction t = incomes.getAt(i);
        
        // KẾT NỐI: Dùng ID để lấy Tên từ các bảng Hash Table
        string sourceName = sources.getSourceName(t.sourceID);
        string walletName = wallets.getWalletName(t.walletID);
        
        printDate(t.day);
        cout << "  " << left << setw(20) << sourceName 
             << left << setw(20) << walletName 
             << right << setw(15) << (size_t)t.amount 
             << "   " << t.description << endl;
    }

    // 2. Hiển thị danh sách chi tiêu
    cout << "\n--- DANH SACH CHI TIEU ---\n";
    cout << left << setw(12) << "Ngay" 
         << left << setw(20) << "Danh Muc" 
         << left << setw(20) << "Vi Tien" 
         << right << setw(15) << "So Tien" 
         << "   Mo ta" << endl;

    for (int i = 0; i < expenses.getCount(); i++) {
        ExpenseTransaction t = expenses.getAt(i);
        
        // KẾT NỐI: Dùng ID để lấy Tên
        string catName = categories.getCategoryName(t.categoryID);
        string walletName = wallets.getWalletName(t.walletID);

        printDate(t.day);
        cout << "  " << left << setw(20) << catName
             << left << setw(20) << walletName
             << right << setw(15) << (size_t)t.amount
             << "   " << t.description << endl;
    }

    // 3. Tổng kết
    double totalIncome = incomes.getTotalIncome();
    double totalExpense = expenses.getTotalExpense();
    cout << "\n---------------------------------------------------";
    cout << "\nTONG THU: " << (size_t)totalIncome;
    cout << "\nTONG CHI: " << (size_t)totalExpense;
    cout << "\nSO DU:    " << (size_t)(totalIncome - totalExpense) << endl;
    cout << "===================================================\n";
}

int main() {
    // Tên các file dữ liệu
    string fileWallet = "saves/wallets.dat";
    string fileSource = "saves/sources.dat";
    string fileCat = "saves/categories.dat";
    string fileInc = "saves/incomes.dat";
    string fileExp = "saves/expenses.dat";
    // --- PHẦN 1: NHẬP DỮ LIỆU MẪU ---
    cout << ">>> DANG KHOI TAO DU LIEU MAU...\n";

    // 1. Khởi tạo các bảng (Size = 10)
    WalletTable wallets(10);
    IncomeSourceTable sources(10);
    CategoryTable categories(10);
    IncomeArray incomes;
    ExpenseArray expenses;

    // 2. Thêm dữ liệu danh mục (Master Data)
    wallets.addWallet(1, "Vi Tien Mat");
    wallets.addWallet(2, "Tai Khoan Techcom");

    sources.insertSource(101, "Luong Thang 12");
    sources.insertSource(102, "Thuong Tet");

    categories.addCategory(201, "An Uong");
    categories.addCategory(202, "Di Chuyen");

    // 3. Thêm giao dịch (Transaction Data)
    // Thu nhập: Lương về ví Techcom
    incomes.addTransaction({15, 12, 2024}, 20000000, 101, 2, "Nhan luong thang 12");
    // Chi tiêu: Ăn trưa bằng tiền mặt
    expenses.addTransaction({16, 12, 2024}, 50000, 201, 1, "Com tam Cali");
    // Chi tiêu: Đổ xăng bằng tiền mặt
    expenses.addTransaction({17, 12, 2024}, 100000, 202, 1, "Do xang xe may");

    // 4. In thử dữ liệu trong RAM
    cout << ">>> DU LIEU TRUOC KHI LUU FILE:\n";
    printReport(incomes, expenses, wallets, sources, categories);


    // --- PHẦN 2: KIỂM TRA LƯU FILE ---
    cout << "\n>>> DANG LUU DU LIEU VAO FILE...\n";
    wallets.saveToFile(fileWallet);
    sources.saveBinary(fileSource);
    categories.saveToFile(fileCat);
    incomes.saveToFile(fileInc);
    expenses.saveToFile(fileExp);
    cout << ">>> DA LUU THANH CONG!\n";


    // --- PHẦN 3: KIỂM TRA ĐỌC FILE (Dữ liệu mới hoàn toàn) ---
    cout << "\n>>> DANG TAI DU LIEU TU FILE LEN CAC DOI TUONG MOI...\n";
    
    // Tạo các đối tượng mới hoàn toàn để đảm bảo không dùng lại dữ liệu cũ trong RAM
    WalletTable newWallets(10);
    IncomeSourceTable newSources(10);
    CategoryTable newCategories(10);
    IncomeArray newIncomes;
    ExpenseArray newExpenses;

    newWallets.loadFromFile(fileWallet);
    newSources.loadBinary(fileSource);
    newCategories.loadFromFile(fileCat);
    newIncomes.loadFromFile(fileInc);
    newExpenses.loadFromFile(fileExp);

    cout << ">>> DU LIEU SAU KHI DOC TU FILE:\n";
    printReport(newIncomes, newExpenses, newWallets, newSources, newCategories);

    return 0;
}