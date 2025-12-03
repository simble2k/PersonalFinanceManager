#ifndef CATEGORY_H
#define CATEGORY_H
#include <string>
#include "NameTable.h"
using namespace std;
class CategoryTable : public NameTable {
public:
    CategoryTable(int size) : NameTable(size) {}
    
    void addCategory(int id, string name) {
        add(id, name);
    }
    
    string getCategoryName(int id) {
        return getName(id);
    }
};
#endif