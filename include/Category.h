#ifndef CATEGORY_H
#define CATEGORY_H
#include <string>
using namespace std;
struct CategoryNode {
    int id;
    string name;
    CategoryNode* next;
    CategoryNode(int id, string name) : id(id), name(name), next(nullptr){};
};
class CategoryTable {
private:
    CategoryNode** table; // Mảng con trỏ
    int capacity;       // Kích thước mảng

    int hashFunction(int key);
public:
    CategoryTable(int initialCapacity);
    ~CategoryTable();
    void addCategory(int id, string name);
    string getCategoryName(int id);
    void removeCategory(int id);
    void saveToFile(const string &filename);
    void loadFromFile(const string &filename);
};
#endif