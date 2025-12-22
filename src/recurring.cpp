#include "../include/recurring.h"
#include "../include/statistic.h"
#include <fstream>
#include <iomanip>

// --- Helper: Kiểm tra năm nhuận và số ngày trong tháng ---
// (Dùng để đảm bảo ngày hợp lệ khi tự động thêm)
int getDaysInMonth(int month, int year) {
    // Feedback 2: Validate month range
    if (month < 1 || month > 12) {
        return 0; // Trả về 0 để báo lỗi
    }

    switch (month) {
        case 4: case 6: case 9: case 11: return 30;
        case 2: return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 29 : 28;
        default: return 31;
    }
}

// 1. Constructor
RecurringManager::RecurringManager(int initialSize) {
    this->capacity = initialSize;
    this->count = 0;
    this->tasks = new RecurringTask[capacity];
}

// 2. Destructor
RecurringManager::~RecurringManager() {
    if (tasks != nullptr) {
        delete[] tasks;
    }
}

// 3. Resize
void RecurringManager::resize() {
    capacity *= 2;
    RecurringTask* newTasks = new RecurringTask[capacity];
    for (int i = 0; i < count; i++) {
        newTasks[i] = tasks[i];
    }
    delete[] tasks;
    tasks = newTasks;
}

// 4. Thêm cấu hình định kỳ
void RecurringManager::addRecurring(bool isIncome, double amount, int catSrcID, int walletID, 
                                    string desc, date start, date end) {
    if (count >= capacity) {
        resize();
    }
    
    tasks[count].id = count + 1; // ID tự tăng đơn giản
    tasks[count].isIncome = isIncome;
    tasks[count].amount = amount;
    tasks[count].categoryOrSourceID = catSrcID;
    tasks[count].walletID = walletID;
    tasks[count].description = desc;
    tasks[count].startDate = start;
    tasks[count].endDate = end;
    
    // Mặc định lastExecuted là 0/0/0 (chưa chạy lần nào)
    tasks[count].lastExecuted = {0, 0, 0}; 
    
    count++;
    cout << "=> Da thiet lap giao dich dinh ky: " << desc << endl;
}

// 5. Logic kiểm tra điều kiện chạy
bool RecurringManager::shouldExecute(const RecurringTask& task, date current) {
    // --- VALIDATION INPUT (Feedback 2) ---
    // Kiểm tra ngày tháng hiện tại có hợp lệ không trước khi xử lý
    if (current.month < 1 || current.month > 12 || current.day < 1) {
        return false;
    }

    // --- KIỂM TRA HIỆU LỰC ---
    if (compareDate(current, task.startDate) < 0) return false;
    if (task.endDate.year != 0 && compareDate(current, task.endDate) > 0) return false;

    if (task.lastExecuted.year != 0) {
        if (task.lastExecuted.month == current.month && task.lastExecuted.year == current.year) {
            return false; // Đã chạy trong tháng này rồi -> Skip
        }
    }

    // --- KIỂM TRA NGÀY KÍCH HOẠT ---
    int daysInCurrentMonth = getDaysInMonth(current.month, current.year);
    
    // Nếu getDaysInMonth trả về 0 (do lỗi tháng), dừng ngay
    if (daysInCurrentMonth == 0) return false;

    int targetDay = task.startDate.day;
    
    // Xử lý logic ngày cuối tháng (VD: set ngày 31 nhưng tháng 2 chỉ có 28)
    if (targetDay > daysInCurrentMonth) {
        targetDay = daysInCurrentMonth;
    }

    // Chỉ chạy nếu hôm nay >= ngày mục tiêu
    if (current.day >= targetDay) {
        return true;
    }

    return false;
}

// 6. Xử lý tự động (Hàm cốt lõi)
void RecurringManager::processRecurring(IncomeArray& incomes, ExpenseArray& expenses) {
    date today = getCurrentDate();
    int runCount = 0;

    cout << "\n>>> DANG KIEM TRA GIAO DICH DINH KY (Thang " << today.month << "/" << today.year << ")...\n";

    for (int i = 0; i < count; i++) {
        if (shouldExecute(tasks[i], today)) {
            // Xác định ngày giao dịch: Lấy ngày hiện tại
            // Lưu ý: Có thể tùy chỉnh để lấy đúng ngày X trong tháng nếu muốn logic phức tạp hơn
            
            string autoDesc = tasks[i].description + " (Auto Monthly)";
            
            if (tasks[i].isIncome) {
                incomes.addTransaction(today, tasks[i].amount, tasks[i].categoryOrSourceID, tasks[i].walletID, autoDesc);
            } else {
                // --- KIỂM TRA SỐ DƯ CHO GIAO DỊCH ĐỊNH KỲ ---
                double currentBal = getWalletBalance(tasks[i].walletID, incomes, expenses);
                
                if (tasks[i].amount > currentBal) {
                    cout << "   [!] BO QUA: Giao dich '" << tasks[i].description 
                         << "' (Vi ID " << tasks[i].walletID << " khong du tien).\n";
                         
                    // Lưu ý: KHÔNG cập nhật lastExecuted để lần sau mở app nó thử lại
                    continue; 
                }

                // Nếu đủ tiền thì mới trừ
                expenses.addTransaction(today, tasks[i].amount, tasks[i].categoryOrSourceID, tasks[i].walletID, autoDesc);
            }
            
            // Cập nhật trạng thái (Chỉ chạy xuống đây nếu transaction thành công)
            tasks[i].lastExecuted = today;
            runCount++;
            cout << "   [OK] Da them giao dich dinh ky: " << tasks[i].description << endl;
        }
    }

    if (runCount == 0) {
        cout << "   [i] Khong co giao dich dinh ky nao can them hom nay.\n";
    } else {
        cout << "   => Da tu dong them " << runCount << " giao dich.\n";
    }
}

RecurringTask RecurringManager::getAt(int index) const {
    if (index < 0 || index >= count) return RecurringTask{};
    return tasks[index];
}

// 7. Lưu file (Binary)
void RecurringManager::saveToFile(const string& filename) {
    ofstream fout(filename, ios::binary);
    if (!fout.is_open()) return;

    fout.write((char*)&count, sizeof(int));

    for (int i = 0; i < count; i++) {
        fout.write((char*)&tasks[i].id, sizeof(int));
        fout.write((char*)&tasks[i].isIncome, sizeof(bool));
        fout.write((char*)&tasks[i].amount, sizeof(double));
        fout.write((char*)&tasks[i].categoryOrSourceID, sizeof(int));
        fout.write((char*)&tasks[i].walletID, sizeof(int));
        
        // Ghi các loại ngày
        fout.write((char*)&tasks[i].startDate, sizeof(date));
        fout.write((char*)&tasks[i].endDate, sizeof(date));
        fout.write((char*)&tasks[i].lastExecuted, sizeof(date));

        // Ghi chuỗi Description
        int len = tasks[i].description.size();
        fout.write((char*)&len, sizeof(int));
        fout.write(tasks[i].description.c_str(), len);
    }
    fout.close();
}

// 8. Đọc file
void RecurringManager::loadFromFile(const string& filename) {
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) return;

    // Reset dữ liệu cũ nếu có
    if (count > 0) count = 0;

    int fileCount;
    fin.read((char*)&fileCount, sizeof(int));

    for (int i = 0; i < fileCount; i++) {
        if (count >= capacity) resize();

        fin.read((char*)&tasks[count].id, sizeof(int));
        fin.read((char*)&tasks[count].isIncome, sizeof(bool));
        fin.read((char*)&tasks[count].amount, sizeof(double));
        fin.read((char*)&tasks[count].categoryOrSourceID, sizeof(int));
        fin.read((char*)&tasks[count].walletID, sizeof(int));

        fin.read((char*)&tasks[count].startDate, sizeof(date));
        fin.read((char*)&tasks[count].endDate, sizeof(date));
        fin.read((char*)&tasks[count].lastExecuted, sizeof(date));

        int len;
        fin.read((char*)&len, sizeof(int));
        char* buf = new char[len + 1];
        fin.read(buf, len);
        buf[len] = '\0';
        tasks[count].description = buf;
        delete[] buf;

        count++;
    }
    fin.close();
}

// 9. In danh sách (Debug/Menu)
void RecurringManager::printRecurringList() {
    if (count == 0) {
        cout << "Chua co cai dat dinh ky nao.\n";
        return;
    }
    cout << "\n--- DANH SACH CAI DAT DINH KY ---\n";
    cout << left << setw(5) << "ID" 
         << left << setw(10) << "Loai"
         << left << setw(15) << "So tien"
         << left << setw(20) << "Mo ta"
         << "Lan chay cuoi" << endl;

    for(int i=0; i<count; i++) {
        cout << left << setw(5) << tasks[i].id
             << left << setw(10) << (tasks[i].isIncome ? "Thu" : "Chi")
             << left << setw(15) << (size_t)tasks[i].amount
             << left << setw(20) << tasks[i].description;
        
        if (tasks[i].lastExecuted.year == 0) cout << "Chua chay lan nao";
        else printDate(tasks[i].lastExecuted);
        
        cout << endl;
    }
}
