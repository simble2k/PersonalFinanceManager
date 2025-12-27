#pragma once

#include <string>
#include <functional>
#include <memory>
#include "raylib.h"
#include "DataManager.h"

#include "date.h"
#include "wallet.h"
#include "IncomeSourceTable.h"
#include "Category.h"
#include "IncomeTransaction.h"
#include "ExpenseTransaction.h"
#include "ScrollArea.h"
#include "button.h"
#include "Dialog.h"
#include "LayoutHelper.h"
#include "Dropdown.h"

using namespace std;

// window settings
const int screenWidth = 1280;
const int screenHeight = 800;

// Dialog settings
const int DIALOG_W = 960;
const int DIALOG_H = 520;

class TransactionWindow {
private:
    // Components
    ScrollArea scLeft; // for incomes
    ScrollArea scRight; // for expenses

    //Add buttons
    Button backBtn;
    Button addIncomeBtn;
    Button addExpenseBtn;

    //Pop-up dialogs
    Dialog addIncomeDialog;
    Dialog addExpenseDialog;
    Dialog confirmDialog;

    Rectangle incomeDialogRect = {0,0,0,0};
    Rectangle expenseDialogRect = {0,0,0,0};
    Rectangle confirmDialogRect = {0,0,0,0};

    std::unique_ptr<Dropdown> incomeWalletDropdown;
    std::unique_ptr<Dropdown> incomeSourceDropdown;
    std::unique_ptr<Dropdown> expenseWalletDropdown;
    std::unique_ptr<Dropdown> expenseCategoryDropdown;

    bool showAddIncomeDialog = false;
    bool showAddExpenseDialog = false;
    bool showConfirmDialog = false;
    bool initialized = false;
    bool confirmingIncome = true;

    std::string confirmMessage;
    DataManager* dataManagerRef = nullptr;
    std::function<void()> onBackRequested; // callback for back button

    // Error indicator for add dialogs (-1 = none)
    int errorID = -1;

    static const int INCOME_AMOUNT_IDX = 0;// Amount is first input
    static const int INCOME_DESC_IDX = 1; // Description is second input
    static const int EXPENSE_AMOUNT_IDX = 0; // Amount is first input
    static const int EXPENSE_DESC_IDX = 1; // Description is second input

    void PopulateWalletDropdown(Dropdown& dropdown);
    void PopulateSourceDropdown(Dropdown& dropdown);
    void PopulateCategoryDropdown(Dropdown& dropdown);
    
    void OpenIncomeDialog();
    void OpenExpenseDialog();
    
    void SubmitIncome();
    void SubmitExpense();
    void CloseDialogs();
    void ShowConfirm(const std::string& message);
    
    void DrawAddIncomeDialog();
    void DrawAddExpenseDialog();
    void DrawConfirmDialog();

    // Dialog input boxes (allocated in Init, freed in destructor)
    Rectangle* dialogBoxes = nullptr;
    
public:
    TransactionWindow();
    ~TransactionWindow();

    void BindDataManager(DataManager* dataManager) { dataManagerRef = dataManager; }
    void SetOnBack(std::function<void()> cb) { onBackRequested = std::move(cb); }

    void Init();
    void Update();
    void Draw(DataManager& dataManager);
};