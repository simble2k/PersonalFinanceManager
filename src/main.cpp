
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdio>
#include "../include/date.h"
#include "../include/wallet.h"
#include "../include/IncomeSourceTable.h"
#include "../include/Category.h"
#include "../include/IncomeTransaction.h"
#include "../include/ExpenseTransaction.h"
#include "../include/statistic.h"
#include "../include/recurring.h"

using namespace std;

// Định nghĩa tên file
const string FILE_WALLET = "saves/wallets.dat";
const string FILE_SOURCE = "saves/sources.dat";
const string FILE_CAT    = "saves/categories.dat";
const string FILE_INC    = "saves/incomes.dat";
const string FILE_EXP    = "saves/expenses.dat";
const string FILE_REC    = "saves/recurring.dat";

void pause() {
    cout << "\nNhan Enter de tiep tuc...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Hàm xóa dữ liệu cũ để test từ đầu
void cleanSaveFiles() {
    remove(FILE_WALLET.c_str());
    remove(FILE_SOURCE.c_str());
    remove(FILE_CAT.c_str());
    remove(FILE_INC.c_str());
    remove(FILE_EXP.c_str());
    remove(FILE_REC.c_str());
    cout << "[TEST] Da xoa toan bo file save cu de bat dau test moi.\n";
}

// --- HÀM MỚI: IN DANH SÁCH ID & TÊN ---
// Hàm này giúp tái sử dụng code, in ra bảng chọn đẹp mắt ở mọi nơi
void printList(NameTable& table, string label) {
    cout << "\n>>> DANH SACH " << label << ":\n";
    cout << "---------------------------------\n";
    
    int count = 0;
    int* ids = table.getAllIDs(count);
    
    if (count == 0) {
        cout << "  (Danh sach trong)\n";
    } else {
        for(int i = 0; i < count; i++) {
            cout << "  ID " << left << setw(5) << ids[i] 
                 << " | " << table.getName(ids[i]) << endl;
        }
    }
    
    delete[] ids;
    cout << "---------------------------------\n";
}

// Hàm Dashboard (Yêu cầu UI)
void printDashboard(WalletTable& wallets, IncomeArray& incomes, ExpenseArray& expenses) {
    cout << "\n========== TONG QUAN TAI CHINH (DASHBOARD) ==========\n";
    
    double totalSystemBalance = 0;
    int walletCount = 0;
    int* walletIDs = wallets.getAllIDs(walletCount);
    
    if (walletCount == 0) cout << "  (Chua co vi nao duoc tao)\n";

    for(int i = 0; i < walletCount; i++) {
        int wid = walletIDs[i];
        string wName = wallets.getWalletName(wid);
        // Sử dụng hàm getWalletBalance từ statistic.h
        double wBalance = getWalletBalance(wid, incomes, expenses);
        
        totalSystemBalance += wBalance;
        cout << "  - " << left << setw(25) << wName 
             << ": " << right << setw(15) << (long long)wBalance << " VND" << endl;
    }
    
    cout << "-----------------------------------------------------\n";
    cout << " TONG TAI SAN (Total Balance): " << right << setw(15) << (long long)totalSystemBalance << " VND\n";
    cout << "=====================================================\n";
    delete[] walletIDs; 
}

void runUserMode() {
    // Load dữ liệu từ file lên RAM
    WalletTable wallets(20);
    IncomeSourceTable sources(20);
    CategoryTable categories(20);
    IncomeArray incomes;
    ExpenseArray expenses;
    RecurringManager recurringMgr;

    wallets.loadFromFile(FILE_WALLET);
    sources.loadFromFile(FILE_SOURCE);
    categories.loadFromFile(FILE_CAT);
    incomes.loadFromFile(FILE_INC);
    expenses.loadFromFile(FILE_EXP);
    recurringMgr.loadFromFile(FILE_REC);

    // Chạy Recurring check ngay khi mở app (Yêu cầu bài toán)
    recurringMgr.processRecurring(incomes, expenses);

    // Nếu lần đầu chưa có ví, tạo ví mặc định
    if (wallets.getName(1) == "Unknown") {
        wallets.addWallet(1, "Tien Mat");
    }

    int choice;
    do {
        // system("cls"); // Bỏ comment nếu chạy trên Windows CMD
        
        // 1. HIỂN THỊ DASHBOARD (Yêu cầu UI)
        printDashboard(wallets, incomes, expenses);

        // 2. MENU ĐIỀU HƯỚNG (Navigation)
        cout << "1. Quan ly Thu Nhap (Income)\n";
        cout << "2. Quan ly Chi Tieu (Expense)\n";
        cout << "3. Quan ly Danh Muc (Master Data)\n";
        cout << "4. Bao cao & Thong ke (Statistics)\n";
        cout << "5. Quan ly giao dich dinh ky (Recurring)\n";
        cout << "0. Luu & Thoat\n";
        cout << "Chon chuc nang: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
        case 1: { // THU NHẬP
            cout << "\n--- THEM THU NHAP ---\n";
            int sid, wid; double amt; string desc; date d = getCurrentDate();
            
            // --- Hiển thị danh sách trước khi nhập ---
            printList(sources, "Nguon Thu");
            cout << "Nhap ID Nguon Thu: "; cin >> sid;
            
            printList(wallets, "Vi Tien");
            cout << "Nhap ID Vi Tien: "; cin >> wid;
            
            cout << "Nhap So Tien: "; cin >> amt;
            cin.ignore();
            cout << "Mo ta: "; getline(cin, desc);
            
            incomes.addTransaction(d, amt, sid, wid, desc);
            cout << "=> Da them thu nhap thanh cong!\n";
            break;
        }
        case 2: { // CHI TIÊU
            cout << "\n--- THEM CHI TIEU ---\n";
            int cid, wid; double amt; string desc; date d = getCurrentDate();
            
            // --- Hiển thị danh sách trước khi nhập ---
            printList(categories, "Danh Muc Chi");
            cout << "Nhap ID Danh Muc: "; cin >> cid;
            
            printList(wallets, "Vi Tien");
            cout << "Nhap ID Vi Tien: "; cin >> wid;
            
            // Kiểm tra số dư (Requirement: Practicality)
            double currentBal = getWalletBalance(wid, incomes, expenses);
            cout << ">> So du hien tai: " << (long long)currentBal << endl;

            cout << "Nhap So Tien: "; cin >> amt;
            if (amt > currentBal) {
                cout << "[LOI] So du khong du!\n";
                pause();
            } else {
                cin.ignore();
                cout << "Mo ta: "; getline(cin, desc);
                expenses.addTransaction(d, amt, cid, wid, desc);
                cout << "=> Da them chi tieu thanh cong!\n";
            }
            break;
        }
        case 3: { // MASTER DATA (Add/Edit/Delete)
            int action, type;
            cout << "\n--- QUAN LY DANH MUC (MASTER DATA) ---\n";
            cout << "1. THEM (Add) | 2. SUA (Edit) | 3. XOA (Delete)\nChon: "; cin >> action;
            cout << "1. Vi (Wallet) | 2. Nguon Thu (Source) | 3. Danh Muc Chi (Category)\nChon: "; cin >> type;

            NameTable* currentTable = nullptr; // Con trỏ đa hình
            string label = "";

            if (type == 1) { currentTable = &wallets; label = "VI TIEN"; }
            else if (type == 2) { currentTable = &sources; label = "NGUON THU"; }
            else if (type == 3) { currentTable = &categories; label = "DANH MUC CHI"; }

            if (currentTable != nullptr) {
                // --- IN DANH SÁCH ---
                printList(*currentTable, label);
                
                int id; string name;
                cout << "Nhap ID can thao tac: "; cin >> id;
                
                if(action != 3) { // Nếu không phải Xóa thì mới cần nhập Tên mới
                    cin.ignore(); 
                    cout << "Nhap Ten: "; getline(cin, name); 
                }

                if(action == 1) currentTable->add(id, name);
                else if(action == 2) currentTable->editName(id, name);
                else if(action == 3) currentTable->remove(id);
                
            } else {
                cout << "[LOI] Lua chon loai du lieu khong hop le!\n";
            }
            break;
        }
        case 4: { // THỐNG KÊ
            statisticMenu(incomes, expenses, wallets, sources, categories);
            break;
        }
        case 5: { // QUẢN LÝ ĐỊNH KỲ
             cout << "\n--- CAI DAT GIAO DICH DINH KY ---\n";
             recurringMgr.printRecurringList();
             
             cout << "\nBan muon them moi khong? (1: Co, 0: Khong): ";
             int rChoice; cin >> rChoice;
             
             if (rChoice == 1) {
                 int type; 
                 cout << "Loai (1-Thu, 2-Chi): "; cin >> type;
                 
                 double amt; int idRef, wid; string desc; 
                 date start;
                 date end = {0, 0, 0}; // Mặc định là vĩnh viễn (Optional)

                 cout << "So tien: "; cin >> amt;
                 
                 // Hiển thị danh sách ID để chọn
                 if(type == 1) printList(sources, "Nguon Thu");
                 else printList(categories, "Danh Muc Chi");
                 cout << "Nhap ID Nguon/Category: "; cin >> idRef;
                 
                 printList(wallets, "Vi Tien");
                 cout << "Nhap ID Vi: "; cin >> wid;
                 
                 cout << "Ngay bat dau (d m y): "; 
                 cin >> start.day >> start.month >> start.year;
                    // --- Nhập ngày kết thúc tùy chọn ---
                 int hasEnd;
                 cout << "Co ngay ket thuc khong? (1: Co, 0: Vinh vien): ";
                 cin >> hasEnd;
                 
                 if (hasEnd == 1) {
                     cout << "Nhap Ngay ket thuc (d m y): ";
                     cin >> end.day >> end.month >> end.year;
                 }
                 // -----------------------------------

                 cin.ignore();
                 cout << "Mo ta: "; getline(cin, desc);
                 
                 recurringMgr.addRecurring((type==1), amt, idRef, wid, desc, start, end);
             }
             break;
        }
        case 0:
            cout << "\n>>> Dang luu du lieu...\n";
            wallets.saveToFile(FILE_WALLET);
            sources.saveToFile(FILE_SOURCE);
            categories.saveToFile(FILE_CAT);
            incomes.saveToFile(FILE_INC);
            expenses.saveToFile(FILE_EXP);
            recurringMgr.saveToFile(FILE_REC);
            cout << "Tam biet!\n";
            break;
        default:
            cout << "Lua chon khong hop le!\n";
        }

    } while (choice != 0);
}

int main() {
    //cleanSaveFiles(); // Bỏ comment dòng này để xóa file save cũ khi test
    runUserMode();

    return 0;
}