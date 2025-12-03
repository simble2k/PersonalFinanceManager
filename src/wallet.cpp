#include "wallet.h"
#include <iostream>
#include <fstream>
using namespace std;
WalletTable::WalletTable(int initialCapacity)
{
    this->capacity = initialCapacity;

    table = new WalletNode *[capacity];

    for (int i = 0; i < capacity; i++)
    {
        table[i] = nullptr;
    }
}
WalletTable::~WalletTable()
{
    for (int i = 0; i < capacity; i++)
    {
        WalletNode *current = table[i];
        while (current != nullptr)
        {
            WalletNode *temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] table;
}
int WalletTable::hashFunction(int key){
    return key % WalletTable::capacity;
}
void WalletTable::addWallet(int id, string name)
{
    int index = hashFunction(id);
    WalletNode *current = table[index];
    while (current != nullptr)
    {
        if (current->id == id)
        {
            current->name = name;
            return;
        }
        current = current->next;
    }

    WalletNode *newwallet = new WalletNode(id, name);
    newwallet->next = table[index];
    table[index] = newwallet;
}
string WalletTable::getWalletName(int id)
{
    int index = hashFunction(id);
    WalletNode *current = table[index];
    while (current != nullptr)
    {
        if (current->id == id)
            return current->name;
        current = current->next;
    }
    return "";
}
void WalletTable::removeWallet(int id)
{
    int index = hashFunction(id);
    WalletNode* current = table[index];
    WalletNode* prev = nullptr;

    while (current != nullptr) {
        if (current->id == id) {
            if (prev == nullptr) {
                table[index] = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}
void WalletTable::saveToFile(const string &filename) {
    ofstream fout(filename, ios::binary | ios::trunc);
    if (!fout.is_open()) {
        return;
    }
    int count = 0;
    for (int i = 0; i < capacity; i++) {
        WalletNode* cur = table[i];
        while (cur) {
            count++;
            cur = cur->next;
        }
    }
    fout.write(reinterpret_cast<char*>(&count), sizeof(count));
    for (int i = 0; i < capacity; i++) {
        WalletNode* cur = table[i];
        while (cur) {
            fout.write(reinterpret_cast<char*>(&cur->id), sizeof(cur->id));
            int nameLen = cur->name.size();
            fout.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            fout.write(cur->name.c_str(), nameLen);
            cur = cur->next;
        }
    }
    fout.close();
}
void WalletTable::loadFromFile(const string &filename) {
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) {
        return;
    }
    for (int i = 0; i < capacity; i++) {
        WalletNode* current = table[i];
        while (current != nullptr) {
            WalletNode* temp = current;
            current = current->next;
            delete temp;
        }
        table[i] = nullptr;
    }
    int count;
    fin.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++) {
        int id;
        fin.read(reinterpret_cast<char*>(&id), sizeof(id));
        int nameLen;
        fin.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        char* nameBuffer = new char[nameLen + 1];
        fin.read(nameBuffer, nameLen);
        nameBuffer[nameLen] = '\0';
        string name(nameBuffer);
        delete[] nameBuffer;
        addWallet(id, name);
    }
    fin.close();
}