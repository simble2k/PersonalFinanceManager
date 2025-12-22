#pragma once

#include "raylib.h"
#include "DataManager.h"
#include "TransactionWindow.h"
#include "dashboard.h"
#include "RecurringWindow.h"
#include "MasterDataWindow.h"

enum Screen { // Active screen
    DASHBOARD,
    TRANSACTION,
    RECURRING,
    MASTER
};

class App {
public:
    App();
    ~App();

    // Initialize app state and resources. Returns false on fatal error.
    bool Init();
    // Update app state (called every frame)
    void Update(float dt);
    // Render the UI (called every frame between BeginDrawing/EndDrawing)
    void Draw();
    // Cleanup resources
    void Shutdown();

private:
    // Data management
    DataManager dataManager_;
    WalletsData data_;

    //Windows management
    TransactionWindow transactionWindow_;
    RecurringWindow recurringWindow_;
    MasterDataWindow masterWindow_;
    Screen currentScreen = DASHBOARD; // Start at dashboard

    bool transactionWindowInitialized = false;
    bool recurringWindowInitialized = false;
    bool masterWindowInitialized = false;
};
