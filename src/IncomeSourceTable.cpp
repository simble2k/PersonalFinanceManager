#include "IncomeSourceTable.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

IncomeSourceTable::IncomeSourceTable(int size)
{
    this->capacity = size;

    table = new SourceNode *[capacity];

    for (int i = 0; i < capacity; i++)
    {
        table[i] = nullptr;
    }
}
IncomeSourceTable:: ~IncomeSourceTable()
{
    for (int i = 0; i < capacity; i++)
    {
        SourceNode *current = table[i];
        while (current != nullptr)
        {
            SourceNode *temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] table;
}
int IncomeSourceTable::hashFunction(int key){
    return key % IncomeSourceTable::capacity;
}
void IncomeSourceTable::insertSource(int id, string name)
{
    int index = hashFunction(id);
    SourceNode *current = table[index];
    while (current != nullptr)
    {
        if (current->id == id)
        {
            current->name = name;
            return;
        }
        current = current->next;
    }

    SourceNode *newincome = new SourceNode(id, name);
    newincome->next = table[index];
    table[index] = newincome;
}
string IncomeSourceTable::getSourceName(int id)
{
    int index = hashFunction(id);
    SourceNode *current = table[index];
    while (current != nullptr)
    {
        if (current->id == id)
            return current->name;
        current = current->next;
    }
    return "No income found under this ID";
}
void IncomeSourceTable::deleteSource(int id) {
    int index = hashFunction(id);
    SourceNode* current = table[index];
    SourceNode* prev = nullptr;

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

void IncomeSourceTable::printAll() {
    cout << "--- DANH SACH NGUON THU ---" << endl;
    for (int i = 0; i < capacity; i++) {
        SourceNode* temp = table[i];
        while (temp != nullptr) {
            cout << "ID: " << temp->id << " | Name: " << temp->name << endl;
            temp = temp->next;
        }
    }
}

void IncomeSourceTable::clear() {
    for (int i = 0; i < capacity; i++) {
        SourceNode* cur = table[i];
        while (cur != nullptr) {
            SourceNode* next = cur->next;
            delete cur;
            cur = next;
        }
        table[i] = nullptr;
    }
}
void IncomeSourceTable::saveBinary(const string &filename) {
    ofstream fout(filename, ios::binary | ios::trunc);
    if (!fout.is_open()) {
        cout << "Cannot open file to save!\n";
        return;
    }

    int count = 0;
    for (int i = 0; i < capacity; i++) {
        SourceNode* cur = table[i];
        while (cur) {
            count++;
            cur = cur->next;
        }
    }

    fout.write((char*)&count, sizeof(int));

    for (int i = 0; i < capacity; i++) {
        SourceNode* cur = table[i];
        while (cur) {
            int id = cur->id;
            int len = cur->name.size();

            fout.write((char*)&id, sizeof(int));

            fout.write((char*)&len, sizeof(int));

            fout.write(cur->name.c_str(), len);

            cur = cur->next;
        }
    }

    fout.close();
}

void IncomeSourceTable::loadBinary(const string &filename) {
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) {
        return;
    }

    clear();

    int count;
    fin.read((char*)&count, sizeof(int));

    for (int i = 0; i < count; i++) {
        int id;
        int len;

        fin.read((char*)&id, sizeof(int));

        fin.read((char*)&len, sizeof(int));

        string name;
        name.resize(len);
        fin.read(&name[0], len);

        insertSource(id, name);
    }

    fin.close();
}