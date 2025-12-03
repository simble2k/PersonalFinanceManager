#ifndef INCOMESOURCETABLE_H
#define INCOMESOURCETABLE_H

#include <string>
using namespace std;

// Struct Node cho Hash Table
struct SourceNode {
    int id;
    string name;
    SourceNode* next;
    SourceNode(int id, string name) : id(id), name(name), next(nullptr){};
};

// Class Hash Table
class IncomeSourceTable {
private:
    SourceNode** table; // Mảng con trỏ
    int capacity;       // Kích thước mảng

    int hashFunction(int key);

public:
    IncomeSourceTable(int size); // Constructor
    ~IncomeSourceTable();              // Destructor

    void insertSource(int id, string name); // Thêm
    string getSourceName(int id);           // Lấy tên
    void deleteSource(int id);              // Xóa
    void printAll();                        // Hiển thị debug

    void saveBinary(const string &filename);
    void loadBinary(const string &filename);

    void clear();
};

#endif