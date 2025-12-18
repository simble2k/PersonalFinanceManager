#ifndef WALLET_H
#define WALLET_H
#include <string>
#include "NameTable.h"

class WalletTable : public NameTable {
public:
    WalletTable(int size) : NameTable(size) {}
    
    void addWallet(int id, std::string name) {
        add(id, name);
    }
    
    std::string getWalletName(int id) {
        return getName(id);
    }
};

#endif