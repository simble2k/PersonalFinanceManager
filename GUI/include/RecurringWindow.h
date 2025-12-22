#pragma once

#include <string>
#include <functional>
#include <memory>

#include "raylib.h"
#include "DataManager.h"
#include "Dialog.h"
#include "Dropdown.h"
#include "ScrollArea.h"
#include "button.h"
#include "LayoutHelper.h"
#include "date.h"

class RecurringWindow {
public:
    RecurringWindow();
    ~RecurringWindow();

    void BindDataManager(DataManager* dataManager) { dataManager_ = dataManager; }
    void SetOnBack(std::function<void()> cb) { onBackRequested_ = std::move(cb); }

    void Init();
    void Update();
    void Draw(DataManager& dataManager);

private:
    // UI components
    ScrollArea listArea_;
    Button backBtn_;
    Button addIncomeBtn_;
    Button addExpenseBtn_;
    Dialog addDialog_;

    // Input layout grid (allocated in Init, freed in destructor)
    Rectangle* inputGrid_ = nullptr;
    int inputCount_ = 0;

    Rectangle dialogRect_ = {0, 0, 0, 0};

    std::unique_ptr<Dropdown> walletDropdown_;
    std::unique_ptr<Dropdown> sourceDropdown_;
    std::unique_ptr<Dropdown> categoryDropdown_;

    bool initialized_ = false;
    bool showAddDialog_ = false;
    bool addingIncome_ = true;

    static const int INPUT_AMOUNT_IDX = 0;
    static const int INPUT_DESC_IDX = 1;
    static const int INPUT_START_IDX = 2;
    static const int INPUT_END_IDX = 3;

    DataManager* dataManager_ = nullptr;
    std::function<void()> onBackRequested_;

    std::string statusMessage_;
    Color statusColor_ = DARKGRAY;

    // Helpers
    void OpenIncomeDialog();
    void OpenExpenseDialog();
    void CloseDialog();
    void SubmitRecurring();

    void PopulateWalletDropdown(Dropdown& dropdown);
    void PopulateSourceDropdown(Dropdown& dropdown);
    void PopulateCategoryDropdown(Dropdown& dropdown);

    bool ParseDate(const std::string& text, date& out, bool allowEmpty = false);
    std::string FormatDate(const date& d) const;
    
    void DrawAddDialog();
    void DrawList(DataManager& dataManager);
};
