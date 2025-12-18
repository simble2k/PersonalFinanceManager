#ifndef INCOMESOURCETABLE_H
#define INCOMESOURCETABLE_H

#include <string>
#include "NameTable.h"
class IncomeSourceTable : public NameTable {

public:
    IncomeSourceTable(int size) : NameTable(size) {}
    
    void insertSource(int id, std::string name) {
        add(id, name);
    }
    
    std::string getSourceName(int id) {
        return getName(id);
    }
};

#endif