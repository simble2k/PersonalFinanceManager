#ifndef NAMETABLE_H
#define NAMETABLE_H

#include <string>
#include <iostream>
#include <fstream>

struct Node {
    int id;
    std::string name;
    Node* next;
    Node(int id, std::string name) : id(id), name(name), next(nullptr) {}
};

class NameTable {
protected:
    Node** table;
    int capacity;
    int hashFunction(int key);

public:
    NameTable(int size);
    virtual ~NameTable(); 

    void add(int id, std::string name);       
    std::string getName(int id);              
    void remove(int id);  
    void editName(int id, std::string name);           
    void saveToFile(const std::string &filename);
    void loadFromFile(const std::string &filename);
    void clear();
    int* getAllIDs(int& count);
};

#endif