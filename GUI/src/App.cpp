#include "App.h"
#include "raylib.h"

App::App() {
}

App::~App() {
    Shutdown();
}

bool App::Init() {
    // Load all data using DataManager
    if (!dataManager_.LoadAllData()) {
        return false; // Fatal error loading data
    }

    // Get WalletsData from DataManager
    data_ = dataManager_.GetWalletsData();

    transactionWindow_.BindDataManager(&dataManager_);
    transactionWindow_.SetOnBack([this]() {
        currentScreen = DASHBOARD;
    });

    recurringWindow_.BindDataManager(&dataManager_);
    recurringWindow_.SetOnBack([this]() {
        currentScreen = DASHBOARD;
    });

    masterWindow_.BindDataManager(&dataManager_);
    masterWindow_.SetOnBack([this]() {
        currentScreen = DASHBOARD;
    });

    statisticWindow_.BindDataManager(&dataManager_);
    statisticWindow_.SetOnBack([this]() {
        currentScreen = DASHBOARD;
    });

    // success
    return true;
}

void App::Update(float dt) {
    // For now, nothing to update global; per-screen updates would go here
    if (currentScreen == TRANSACTION && transactionWindowInitialized) {
        transactionWindow_.Update();
    }
    if (currentScreen == RECURRING && recurringWindowInitialized) {
        recurringWindow_.Update();
    }
    if (currentScreen == MASTER && masterWindowInitialized) {
        masterWindow_.Update();
    }
    if (currentScreen == STATISTICS && statisticWindowInitialized) {
        statisticWindow_.Update();
    }
}

void App::Draw() {
    switch (currentScreen) {
    case DASHBOARD:
        // Refresh wallet data when showing dashboard
        data_ = dataManager_.GetWalletsData();
        drawDashboard(data_, [this](int index) {
            if (index == 0) { // Add Transaction
                if (!transactionWindowInitialized) {
                    transactionWindow_.Init();
                    transactionWindowInitialized = true;
                }
                currentScreen = TRANSACTION;
            }
            if (index == 1) { // Reports & Statistics
                if (!statisticWindowInitialized) {
                    statisticWindow_.Init();
                    statisticWindowInitialized = true;
                }
                currentScreen = STATISTICS;
            }
            if (index == 2) { // Master Data Center
                if (!masterWindowInitialized) {
                    masterWindow_.Init();
                    masterWindowInitialized = true;
                }
                currentScreen = MASTER;
            }
            if (index == 3) { // Recurring Transactions
                if (!recurringWindowInitialized) {
                    recurringWindow_.Init();
                    recurringWindowInitialized = true;
                }
                currentScreen = RECURRING;
            }
            if(index == 4) { // EXIT Application
                CloseWindow();
            }
        });
        break;
    case TRANSACTION:
        if (transactionWindowInitialized) {
            transactionWindow_.Draw(dataManager_);
        }
        break;
    case RECURRING:
        if (recurringWindowInitialized) {
            recurringWindow_.Draw(dataManager_);
        }
        break;
    case MASTER:
        if (masterWindowInitialized) {
            masterWindow_.Draw(dataManager_);
        }
        break;
    case STATISTICS:
        if (statisticWindowInitialized) {
            statisticWindow_.Draw(dataManager_);
        }
        break;
    }
}

void App::Shutdown() {
    // Save all data before shutting down
    dataManager_.SaveAllData();

    // Clean up the dynamic array
    if (data_.wallets) {
        delete[] data_.wallets;
        data_.wallets = nullptr;
        data_.walletCount = 0;
    }
}
