#pragma once
// For loading and saving data

#include "raylib.h"
#include "wallet.h"
#include "ExpenseTransaction.h"
#include "IncomeTransaction.h"
#include "IncomeSourceTable.h"
#include "recurring.h"
#include "Category.h"

#include <string>

// Minimal Wallet struct used by dashboard/transaction window code
struct Wallet {
    std::string name;
    double balance;
    Wallet() : name("Unknown"), balance(0.0) {}
    Wallet(const std::string &n, double b) : name(n), balance(b) {}
};

struct WalletsData {
    double totalBalance;
    int walletCount;
    Wallet *wallets; // pointer to an array 
    WalletsData() : totalBalance(0.0), walletCount(0), wallets(nullptr) {}
};

class DataManager {
public:
    DataManager(); // Constructor
    ~DataManager(); // Destructor

    bool LoadAllData(const std::string& saveDir = "saves/"); // default save directory is "saves/"
    bool SaveAllData(const std::string& saveDir = "saves/");

    // Compute and return WalletsData based on loaded data used for Dashboard
    WalletsData GetWalletsData();
    WalletTable wallets_;
    
    IncomeArray incomes_;
    ExpenseArray expenses_;
    IncomeSourceTable sources_;
    CategoryTable categories_;
    RecurringManager recurring_;
};

