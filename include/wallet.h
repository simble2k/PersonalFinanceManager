#ifndef WALLET_H
#define WALLET_H
#include <string>
using namespace std;

struct WalletNode {
    int id;
    string name;
    WalletNode* next;
    WalletNode(int id, string name) : id(id), name(name), next(nullptr){};
};
class WalletTable {
private:
    WalletNode** table; // Mảng con trỏ
    int capacity;       // Kích thước mảng

    int hashFunction(int key);

public:
    WalletTable(int initialCapacity);
    ~WalletTable();

    void addWallet(int id, string name);
    string getWalletName(int id);
    void removeWallet(int id);
    void saveToFile(const string &filename);
    void loadFromFile(const string &filename);
};

#endif