#ifndef NAMETABLE_H
#define NAMETABLE_H

#include <string>
#include <iostream>
#include <fstream>
using namespace std;

struct Node {
    int id;
    string name;
    Node* next;
    Node(int id, string name) : id(id), name(name), next(nullptr) {}
};

class NameTable {
protected:
    Node** table;
    int capacity;
    int hashFunction(int key);

public:
    NameTable(int size = 10);  // Default size of 10 for hash table buckets
    virtual ~NameTable(); 

    void add(int id, string name);       
    string getName(int id);              
    void remove(int id);  
    void editName(int id, string name);           
    void saveToFile(const string &filename);
    void loadFromFile(const string &filename);
    void clear();
    int* getAllIDs(int& count);
};

#endif