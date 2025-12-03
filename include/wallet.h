#ifndef WALLET_H
#define WALLET_H
#include <string>
#include "NameTable.h"
using namespace std;

class WalletTable : public NameTable {
public:
    WalletTable(int size) : NameTable(size) {}
    
    void addWallet(int id, string name) {
        add(id, name);
    }
    
    string getWalletName(int id) {
        return getName(id);
    }
};

#endif