#ifndef INCOMESOURCETABLE_H
#define INCOMESOURCETABLE_H

#include <string>
#include "NameTable.h"
using namespace std;

class IncomeSourceTable : public NameTable {

public:
    IncomeSourceTable(int size) : NameTable(size) {}
    
    void insertSource(int id, string name) {
        add(id, name);
    }
    
    string getSourceName(int id) {
        return getName(id);
    }
};

#endif