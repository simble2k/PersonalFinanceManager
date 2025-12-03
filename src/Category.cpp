#include "Category.h"
#include <iostream>
#include <fstream>
using namespace std;
CategoryTable::CategoryTable(int initialCapacity)
{
    this->capacity = initialCapacity;

    table = new CategoryNode *[capacity];

    for (int i = 0; i < capacity; i++)
    {
        table[i] = nullptr;
    }
}
CategoryTable::~CategoryTable()
{
    for (int i = 0; i < capacity; i++)
    {
        CategoryNode *current = table[i];
        while (current != nullptr)
        {
            CategoryNode *temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] table;
}
int CategoryTable::hashFunction(int key){
    return key % CategoryTable::capacity;
}
void CategoryTable::addCategory(int id, string name)
{
    int index = hashFunction(id);
    CategoryNode *current = table[index];
    while (current != nullptr)
    {
        if (current->id == id)
        {
            current->name = name;
            return;
        }
        current = current->next;
    }

    CategoryNode *newcategory = new CategoryNode(id, name);
    newcategory->next = table[index];
    table[index] = newcategory;
}
string CategoryTable::getCategoryName(int id)
{
    int index = hashFunction(id);
    CategoryNode *current = table[index];
    while (current != nullptr)
    {
        if (current->id == id)
            return current->name;
        current = current->next;
    }
    return "";
}
void CategoryTable::removeCategory(int id)
{
    int index = hashFunction(id);
    CategoryNode* current = table[index];
    CategoryNode* prev = nullptr;

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
void CategoryTable::saveToFile(const string &filename) {
    ofstream fout(filename, ios::binary | ios::trunc);
    if (!fout.is_open()) {
        return;
    }
    int count = 0;
    for (int i = 0; i < capacity; i++) {
        CategoryNode* current = table[i];
        while (current != nullptr) {
            count++;
            current = current->next;
        }
    }
    fout.write(reinterpret_cast<char*>(&count), sizeof(count));
    for (int i = 0; i < capacity; i++) {
        CategoryNode* current = table[i];
        while (current != nullptr) {
            fout.write(reinterpret_cast<char*>(&current->id), sizeof(current->id));
            int nameLength = current->name.size();
            fout.write(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
            fout.write(current->name.c_str(), nameLength);
            current = current->next;
        }
    }
    fout.close();
}
void CategoryTable::loadFromFile(const string &filename) {
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) {
        return;
    }
    for (int i = 0; i < capacity; i++) {
        CategoryNode* current = table[i];
        while (current != nullptr) {
            CategoryNode* temp = current;
            current = current->next;
            delete temp;
        }
        table[i] = nullptr;
    }
    int count = 0;
    fin.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++) {
        int id;
        fin.read(reinterpret_cast<char*>(&id), sizeof(id));
        int nameLength;
        fin.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        char* buffer = new char[nameLength + 1];
        fin.read(buffer, nameLength);
        buffer[nameLength] = '\0';
        string name(buffer);
        delete[] buffer;
        addCategory(id, name);
    }
    fin.close();
}

