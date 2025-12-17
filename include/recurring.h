#ifndef RECURRING_H
#define RECURRING_H

#include <string>
#include <iostream>
#include "date.h"
#include "IncomeTransaction.h"
#include "ExpenseTransaction.h"

using namespace std;

// Cấu trúc một tác vụ định kỳ
struct RecurringTask {
    int id;                 // ID quản lý
    bool isIncome;          // true = Thu nhập, false = Chi tiêu
    double amount;          // Số tiền
    int categoryOrSourceID; // ID Nguồn (nếu thu) hoặc Danh mục (nếu chi)
    int walletID;           // ID Ví
    string description;     // Mô tả (VD: Tien thue nha)
    
    date startDate;         // Ngày bắt đầu hiệu lực
    date endDate;           // Ngày kết thúc (nếu year=0 coi như vĩnh viễn)
    date lastExecuted;      // Ngày cuối cùng hệ thống đã tự động thêm
};

// Class quản lý danh sách định kỳ (Dynamic Array)
class RecurringManager {
private:
    RecurringTask* tasks;   // Mảng động chứa các tác vụ
    int capacity;           // Sức chứa
    int count;              // Số lượng hiện tại

    void resize();          // Hàm mở rộng bộ nhớ

    // Hàm hỗ trợ kiểm tra xem có cần chạy task trong tháng này không
    bool shouldExecute(const RecurringTask& task, date currentDate);

public:
    RecurringManager(int initialSize = 10);
    ~RecurringManager();

    // Thêm một cấu hình định kỳ mới
    void addRecurring(bool isIncome, double amount, int catSrcID, int walletID, 
                      string desc, date start, date end = {0,0,0});

    // Hàm QUAN TRỌNG NHẤT: Quét và tự động thêm giao dịch
    // Cần truyền tham chiếu IncomeArray và ExpenseArray để thêm dữ liệu vào đó
    void processRecurring(IncomeArray& incomes, ExpenseArray& expenses);

    // Lưu và Đọc file (để nhớ trạng thái lastExecuted)
    void saveToFile(const string& filename);
    void loadFromFile(const string& filename);
    
    // Debug: Xem danh sách đang cài đặt
    void printRecurringList();
};

#endif