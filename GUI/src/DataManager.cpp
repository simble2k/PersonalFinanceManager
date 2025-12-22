#include "DataManager.h"
#include <filesystem>  // For std::filesystem (C++17)
#include <iostream>    // For std::cout (optional, for debugging)

DataManager::DataManager() : wallets_(10), incomes_(10), expenses_(10), sources_(10), categories_(10), recurring_(10) {
    // All members now have default constructors with size 10
}

DataManager::~DataManager() {
    // Destructors of members will handle cleanup
}

bool DataManager::LoadAllData(const std::string& saveDir) { // default save directory is "saves/"
    try {
        // Ensure the save directory exists; create it if not, use directly if exists
        std::filesystem::create_directories(saveDir);

        // Load each data type from its file
        // File paths are constructed by appending filename to saveDir
        wallets_.loadFromFile(saveDir + "wallets.dat");
        incomes_.loadFromFile(saveDir + "incomes.dat");
        expenses_.loadFromFile(saveDir + "expenses.dat");
        sources_.loadFromFile(saveDir + "sources.dat");
        categories_.loadFromFile(saveDir + "categories.dat");
        recurring_.loadFromFile(saveDir + "recurring.dat");

        // Seed a default wallet for first-time users
        int walletCount = 0;
        int* ids = wallets_.getAllIDs(walletCount);
        delete[] ids;
        if (walletCount == 0) {
            wallets_.addWallet(1, "Cash");
            SaveAllData(saveDir);
        }

        // If all loads succeed, return true
        return true;
    } catch (const std::exception& e) {
        // If any load fails,catch, return false and print error for debugging
        std::cout << "Error loading data: " << e.what() << std::endl;
        return false;
    }
}

bool DataManager::SaveAllData(const std::string& saveDir) { // default save directory is "saves/"
    try {
        // Ensure the save directory exists; create it if not
        std::filesystem::create_directories(saveDir);

        // Save each data type to its file
        wallets_.saveToFile(saveDir + "wallets.dat");
        incomes_.saveToFile(saveDir + "incomes.dat");
        expenses_.saveToFile(saveDir + "expenses.dat");
        sources_.saveToFile(saveDir + "sources.dat");
        categories_.saveToFile(saveDir + "categories.dat");
        recurring_.saveToFile(saveDir + "recurring.dat");

        // If all saves succeed, return true
        return true;
    } catch (const std::exception& e) {
        // If any save fails, catch, return false and print error for debugging
        std::cout << "Error saving data: " << e.what() << std::endl;
        return false;
    }
}

WalletsData DataManager::GetWalletsData() {
    WalletsData walletsData;

    // Get wallet count and IDs
    int idCount = 0;
    int* ids = wallets_.getAllIDs(idCount);

    // Allocate Wallet array
    Wallet* walletArray = new Wallet[idCount];
    double totalBalance = 0.0;

    for (int i = 0; i < idCount; ++i) {
        int id = ids[i];
        std::string name = wallets_.getName(id);
        
        double balance = 0.0; // TODO: Compute actual balance from incomes/expenses for this wallet
        walletArray[i] = Wallet(name, balance);
        totalBalance += balance; // Sum wallet balances
    }

    delete[] ids;

    // Alternatively, compute totalBalance as total incomes - total expenses
    double totalIncomes = incomes_.getTotalIncome();
    double totalExpenses = expenses_.getTotalExpense();

    // Struct values
    walletsData.totalBalance = totalIncomes - totalExpenses;
    walletsData.walletCount = idCount;
    walletsData.wallets = walletArray; //Point to allocated array

    return walletsData;
}
