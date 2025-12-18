#include "ExpenseTransaction.h"
#include <fstream>
#include <iostream>
using namespace std;
ExpenseArray::ExpenseArray(int initialSize)
{
    capacity = initialSize;
    count = 0;
    list = new ExpenseTransaction[capacity];
}
ExpenseArray::~ExpenseArray()
{
    delete[] list;
}
void ExpenseArray::resize()
{
    capacity *= 2;
    ExpenseTransaction* newList = new ExpenseTransaction[capacity];
    for (int i = 0; i < count; i++)
    {
        newList[i] = list[i];
    }
    delete[] list;
    list = newList;
}
void ExpenseArray::addTransaction(date day, double amount, int categoryID, int walletID, string desc)
{
    if (count == capacity)
    {
        resize();
    }
    list[count].day = day;
    list[count].amount = amount;
    list[count].categoryID = categoryID;
    list[count].walletID = walletID;
    list[count].description = desc;
    count++;
}
int ExpenseArray::getCount()
{
    return count;
}
ExpenseTransaction ExpenseArray::getAt(int index)
{
    if (index < 0 || index >= count)
    {
        throw out_of_range("Index out of range");
    }
    return list[index];
}
double ExpenseArray::getTotalExpense()
{
    double total = 0.0;
    for (int i = 0; i < count; i++)
    {
        total += list[i].amount;
    }
    return total;
}
void ExpenseArray::saveToFile(const string &filename)
{
    ofstream fout(filename, ios::binary);
    if (!fout.is_open())
    {
        return;
    }
    fout.write(reinterpret_cast<char*>(&count), sizeof(count));
    for (int i = 0; i < count; i++)
    {
        fout.write(reinterpret_cast<char*>(&list[i].day), sizeof(date));
        fout.write(reinterpret_cast<char*>(&list[i].amount), sizeof(double));
        fout.write(reinterpret_cast<char*>(&list[i].categoryID), sizeof(int));
        fout.write(reinterpret_cast<char*>(&list[i].walletID), sizeof(int));

        int descLen = list[i].description.size();
        fout.write(reinterpret_cast<char*>(&descLen), sizeof(descLen));
        fout.write(list[i].description.c_str(), descLen);
    }
    fout.close();
}
void ExpenseArray::loadFromFile(const string &filename)
{
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) return;
    
    fin.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (count > capacity) {
        delete[] list;
        capacity = count;
        list = new ExpenseTransaction[capacity];
    }    
    for (int i = 0; i < count; i++)
    {
        fin.read(reinterpret_cast<char*>(&list[i].day), sizeof(date));
        fin.read(reinterpret_cast<char*>(&list[i].amount), sizeof(double));
        fin.read(reinterpret_cast<char*>(&list[i].categoryID), sizeof(int));
        fin.read(reinterpret_cast<char*>(&list[i].walletID), sizeof(int));

        int descLen;
        fin.read(reinterpret_cast<char*>(&descLen), sizeof(descLen));
        
        // FIX: Validate length
        if (descLen < 0 || descLen > 10000) {
            list[i].description = "";
        } else {
            list[i].description.resize(descLen);
            fin.read(&list[i].description[0], descLen);
        }
    }
    fin.close();
}