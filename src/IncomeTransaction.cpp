#include "IncomeTransaction.h"
#include <fstream>
#include <iostream>
using namespace std;
IncomeArray::IncomeArray(int initialSize)
{
    capacity = initialSize;
    count = 0;
    list = new IncomeTransaction[capacity];
}
IncomeArray::~IncomeArray()
{
    delete[] list;
}
void IncomeArray::resize()
{
    capacity *= 2;
    IncomeTransaction* newList = new IncomeTransaction[capacity];
    for (int i = 0; i < count; i++)
    {
        newList[i] = list[i];
    }
    delete[] list;
    list = newList;
}
void IncomeArray::addTransaction(date day, double amount, int sourceID, int walletID, string desc)
{
    if (count == capacity)
    {
        resize();
    }
    list[count].day = day;
    list[count].amount = amount;
    list[count].sourceID = sourceID;
    list[count].walletID = walletID;
    list[count].description = desc;
    count++;
}
int IncomeArray::getCount()
{
    return count;
}
IncomeTransaction IncomeArray::getAt(int index)
{
    if (index < 0 || index >= count)
    {
        throw out_of_range("Index out of range");
    }
    return list[index];
}
double IncomeArray::getTotalIncome()
{
    double total = 0.0;
    for (int i = 0; i < count; i++)
    {
        total += list[i].amount;
    }
    return total;
}
void IncomeArray::saveToFile(const string &filename)
{
    ofstream fout(filename, ios::binary);
    if (!fout.is_open())
    {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }
    fout.write((char*)&count, sizeof(int));
    for (int i = 0; i < count; i++)
    {
        fout.write((char*)&list[i].day, sizeof(date));
        fout.write((char*)&list[i].amount, sizeof(double));
        fout.write((char*)&list[i].sourceID, sizeof(int));
        fout.write((char*)&list[i].walletID, sizeof(int));

        int descLen = list[i].description.size();
        fout.write((char*)&descLen, sizeof(int));
        fout.write(list[i].description.c_str(), descLen);
    }
    fout.close();
}
void IncomeArray::loadFromFile(const string &filename)
{
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) return;
    
    fin.read((char*)&count, sizeof(int));
    if (count > capacity) {
        delete[] list;
        capacity = count;
        list = new IncomeTransaction[capacity];
    }
    for (int i = 0; i < count; i++)
    {
        fin.read((char*)&list[i].day, sizeof(date));
        fin.read((char*)&list[i].amount, sizeof(double));
        fin.read((char*)&list[i].sourceID, sizeof(int));
        fin.read((char*)&list[i].walletID, sizeof(int));

        int descLen;
        fin.read((char*)&descLen, sizeof(int));
        
        if (descLen < 0 || descLen > 10000) {
            list[i].description = "";
        } else {
            list[i].description.resize(descLen);
            fin.read(&list[i].description[0], descLen);
        }
    }
    fin.close();
}


