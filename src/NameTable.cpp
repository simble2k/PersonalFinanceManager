#include "NameTable.h"
#include <iostream>
#include <fstream>
using namespace std;

NameTable::NameTable(int size)
{
    this->capacity = size;

    table = new Node *[capacity];

    for (int i = 0; i < capacity; i++)
    {
        table[i] = nullptr;
    }
}
NameTable::~NameTable()
{
    clear();
    delete[] table;
}
int NameTable::hashFunction(int key){
    return key % capacity;
}
void NameTable::add(int id, string name)
{
    int index = hashFunction(id);
    Node *current = table[index];
    while (current != nullptr)
    {
        if (current->id == id)
        {
            cout << "[Loi] ID " << id << " da ton tai! Khong the them.\n";
            return;
        }
        current = current->next;
    }

    Node *newNode = new Node(id, name);
    newNode->next = table[index];
    table[index] = newNode;
    cout << "=> Da them thanh cong: " << name << " (ID: " << id << ")\n";
}
string NameTable::getName(int id)
{
    int index = hashFunction(id);
    Node *current = table[index];
    while (current != nullptr)
    {
        if (current->id == id)
            return current->name;
        current = current->next;
    }
    return "Unknown";
}
void NameTable::remove(int id){
    int index = hashFunction(id);
    Node* current = table[index];
    Node* prev = nullptr;

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
void NameTable::saveToFile(const string &filename) {
    ofstream fout(filename, ios::binary | ios::trunc);
    if (!fout.is_open()) {
        return;
    }   
    int count = 0;
    for (int i = 0; i < capacity; i++) {
        Node* cur = table[i];
        while (cur) {
            count++;
            cur = cur->next;
        }
    }
    fout.write((char*)&count, sizeof(int));
    for (int i = 0; i < capacity; i++) {
        Node* cur = table[i];
        while (cur) {
            int id = cur->id;
            int nameLen = (int)cur->name.size();
            fout.write((char*)&id, sizeof(int));
            fout.write((char*)&nameLen, sizeof(int));
            fout.write(cur->name.c_str(), nameLen);
            cur = cur->next;
        }
    }
    fout.close();
}
void NameTable::loadFromFile(const string &filename) {
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) return; 
    
    clear();
    int count = 0;  
    fin.read((char*)&count, sizeof(int));
    for (int i = 0; i < count; i++) {
        int id;
        int nameLen;
        fin.read((char*)&id, sizeof(int));
        fin.read((char*)&nameLen, sizeof(int));
        
        string name;
        if (nameLen < 0 || nameLen > 10000) {
            name = "Invalid";
        } else {
            name.resize(nameLen);
            fin.read(&name[0], nameLen);
        }
        add(id, name);
    }
    fin.close();
}
void NameTable::clear() {
    for (int i = 0; i < capacity; i++) {
        Node* cur = table[i];
        while (cur != nullptr) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        table[i] = nullptr;
    }
}
void NameTable::editName(int id, string newName) {
    int index = hashFunction(id);
    Node* current = table[index];
    
    while (current != nullptr) {
        if (current->id == id) {
            current->name = newName;
            cout << "=> Da cap nhat ID " << id << " thanh ten: " << newName << endl;
            return;
        }
        current = current->next;
    }
    cout << "[Loi] Khong tim thay ID " << id << " de sua.\n";
}
int* NameTable::getAllIDs(int& count) {
    // 1. Đếm số lượng phần tử trước
    count = 0;
    for (int i = 0; i < capacity; i++) {
        Node* cur = table[i];
        while (cur != nullptr) {
            count++;
            cur = cur->next;
        }
    }

    // 2. Cấp phát mảng để chứa ID
    int* list = new int[count];
    int index = 0;

    // 3. Đổ dữ liệu vào mảng
    for (int i = 0; i < capacity; i++) {
        Node* cur = table[i];
        while (cur != nullptr) {
            list[index++] = cur->id;
            cur = cur->next;
        }
    }

    return list;
}