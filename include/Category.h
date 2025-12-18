#ifndef CATEGORY_H
#define CATEGORY_H
#include <string>
#include "NameTable.h"
class CategoryTable : public NameTable {
public:
    CategoryTable(int size) : NameTable(size) {}
    
    void addCategory(int id, std::string name) {
        add(id, name);
    }
    
    std::string getCategoryName(int id) {
        return getName(id);
    }
};
#endif