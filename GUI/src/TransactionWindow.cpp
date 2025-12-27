#include "TransactionWindow.h"
#include "ScrollArea.h"
#include "Helper.h"
#include "raylib.h"
#include <string>
#include <cstdio>

// Layout constants tuned for 1280x800 window
static const int TABLE_START_Y = 180;
static const int PADDING = 24;
static const int TITLE_BOX_W = 520;
static const int TITLE_BOX_H = 72;
static const int CTRL_BOX_W = 200;
static const int CTRL_BOX_H = 46;
static const int CONTROL_SPACING = 12;
static const int COL_GAP = 20;
static const int LEFT_COL_X = PADDING;
static const int LEFT_COL_W = (screenWidth - PADDING * 2 - COL_GAP) / 2;
static const int RIGHT_COL_X = LEFT_COL_X + LEFT_COL_W + COL_GAP;
static const int RIGHT_COL_W = LEFT_COL_W;
static const int TABLE_H = 520;
static const int ROW_HEIGHT = 36;

// Constructor
TransactionWindow::TransactionWindow() {
}

// Destructor
TransactionWindow::~TransactionWindow() {
    // Cleanup if needed
    delete[] dialogBoxes;
}

void TransactionWindow::Init() {
    if (initialized) return;

    scLeft.Init((float)LEFT_COL_X, (float)TABLE_START_Y, (float)LEFT_COL_W, (float)TABLE_H);
    scRight.Init((float)RIGHT_COL_X, (float)TABLE_START_Y, (float)RIGHT_COL_W, (float)TABLE_H);

    backBtn = Button((float)PADDING, 20.0f, 140.0f, (float)CTRL_BOX_H, "< Dashboard",
                     LIGHTGRAY, GRAY, BLACK, [this]() {
                         if (onBackRequested) onBackRequested();
                     });

    float ctrlY = 20.0f;
    float expenseX = (float)(screenWidth - PADDING - CTRL_BOX_W);
    float incomeX = expenseX - CONTROL_SPACING - CTRL_BOX_W;

    addIncomeBtn = Button(incomeX, ctrlY, CTRL_BOX_W, CTRL_BOX_H, "Add Income", GREEN, DARKGREEN, WHITE,
                          [this]() { OpenIncomeDialog(); });
    addExpenseBtn = Button(expenseX, ctrlY, CTRL_BOX_W, CTRL_BOX_H, "Add Expense", GREEN, DARKGREEN, WHITE,
                           [this]() { OpenExpenseDialog(); });

    incomeDialogRect = LayoutHelper::CenterRect(0, 0, (float)screenWidth, (float)screenHeight, DIALOG_W, DIALOG_H);
    expenseDialogRect = incomeDialogRect;
    confirmDialogRect = LayoutHelper::CenterRect(0, 0, (float)screenWidth, (float)screenHeight, 420.0f, 220.0f);

    // Build dialog grids (2 rows x 2 cols: top = wallet/source, bottom = amount/description)
    if (dialogBoxes) {
        delete[] dialogBoxes;
        dialogBoxes = nullptr;
    }
    dialogBoxes = LayoutHelper::CreateGrid(40, 130, 2, 2, 320.0f, 44.0f, 40.0f);

    addIncomeDialog = Dialog(incomeDialogRect.x, incomeDialogRect.y, incomeDialogRect.width, incomeDialogRect.height, "Add Income");

    // Setup text inputs and buttons (positions are relative to dialog origin)
    addIncomeDialog.AddTextInput(dialogBoxes[2].x, dialogBoxes[2].y, dialogBoxes[2].width, dialogBoxes[2].height, "Amount");
    addIncomeDialog.AddTextInput(dialogBoxes[3].x, dialogBoxes[3].y, dialogBoxes[3].width, dialogBoxes[3].height, "Description");
    // Character caps
    addIncomeDialog.SetInputMaxLength(INCOME_AMOUNT_IDX, 20);
    addIncomeDialog.SetInputMaxLength(INCOME_DESC_IDX, 120);

    addIncomeDialog.AddButton(incomeDialogRect.width - 220, incomeDialogRect.height - 70, 100, 46, "Add",
                              GREEN, DARKGREEN, WHITE, [this]() { SubmitIncome(); });
    addIncomeDialog.AddButton(incomeDialogRect.width - 110, incomeDialogRect.height - 70, 100, 46, "Cancel",
                              RED, MAROON, WHITE, [this]() { CloseDialogs(); });

    
                              addExpenseDialog = Dialog(expenseDialogRect.x, expenseDialogRect.y, expenseDialogRect.width, expenseDialogRect.height, "Add Expense");
    addExpenseDialog.AddTextInput(dialogBoxes[2].x, dialogBoxes[2].y, dialogBoxes[2].width, dialogBoxes[2].height, "Amount");
    addExpenseDialog.AddTextInput(dialogBoxes[3].x, dialogBoxes[3].y, dialogBoxes[3].width, dialogBoxes[3].height, "Description");
    // Character caps
    addExpenseDialog.SetInputMaxLength(EXPENSE_AMOUNT_IDX, 20);
    addExpenseDialog.SetInputMaxLength(EXPENSE_DESC_IDX, 120);
    addExpenseDialog.AddButton(expenseDialogRect.width - 220, expenseDialogRect.height - 70, 100, 46, "Add",
                               GREEN, DARKGREEN, WHITE, [this]() { SubmitExpense(); });
    addExpenseDialog.AddButton(expenseDialogRect.width - 110, expenseDialogRect.height - 70, 100, 46, "Cancel",
                               RED, MAROON, WHITE, [this]() { CloseDialogs(); });

    
                               confirmDialog = Dialog(confirmDialogRect.x, confirmDialogRect.y, confirmDialogRect.width, confirmDialogRect.height, "Saved");
    confirmDialog.AddButton(24, confirmDialogRect.height - 70, 160, 44, "Keep adding",
                            GREEN, DARKGREEN, WHITE, [this]() {
                                showConfirmDialog = false;
                                confirmDialog.Close();
                                confirmingIncome ? OpenIncomeDialog() : OpenExpenseDialog();
                            });
    confirmDialog.AddButton(confirmDialogRect.width - 184, confirmDialogRect.height - 70, 160, 44, "Dashboard",
                            SKYBLUE, BLUE, WHITE, [this]() {
                                showConfirmDialog = false;
                                confirmDialog.Close();
                                if (onBackRequested) onBackRequested();
                            });

    initialized = true;
}

void TransactionWindow::Update() {
    if (!initialized) Init();

    // Only update buttons if no dialog is open
    if (!showAddIncomeDialog && !showAddExpenseDialog && !showConfirmDialog) {
        backBtn.Update();
        addIncomeBtn.Update();
        addExpenseBtn.Update();
    }

    if (showAddIncomeDialog) {
        addIncomeDialog.Update();
        if (incomeWalletDropdown) incomeWalletDropdown->Update();
        if (incomeSourceDropdown) incomeSourceDropdown->Update();
    }
    if (showAddExpenseDialog) {
        addExpenseDialog.Update();
        if (expenseWalletDropdown) expenseWalletDropdown->Update();
        if (expenseCategoryDropdown) expenseCategoryDropdown->Update();
    }

    if (showConfirmDialog) {
        confirmDialog.Update();
    }
}

void TransactionWindow::Draw(DataManager& dataManager) {
    if (!initialized) Init();

    dataManagerRef = &dataManager;

    IncomeArray& incomes = dataManager.incomes_;
    ExpenseArray& expenses = dataManager.expenses_;
    IncomeSourceTable& sources = dataManager.sources_;
    CategoryTable& categories = dataManager.categories_;

    // --- TOP AREA ---
    Rectangle titleBox = { (float)PADDING, 10.0f, (float)(screenWidth - PADDING * 2), (float)TITLE_BOX_H };
    DrawRectangleRec(titleBox, Fade(SKYBLUE, 0.10f));
    DrawRectangleLinesEx(titleBox, 2.0f, DARKBLUE);
    LayoutHelper::MeasureAndDrawText("Transaction", 32, DARKBLUE, titleBox.x + PADDING + 140, titleBox.y + 20);

    backBtn.Draw();
    addIncomeBtn.Draw();
    addExpenseBtn.Draw();

    // --- TABLE AREA ---
    // Table titles
    LayoutHelper::MeasureAndDrawText("Incomes", 22, DARKGREEN, LEFT_COL_X + PADDING, TABLE_START_Y - 32);
    LayoutHelper::MeasureAndDrawText("Expenses", 22, MAROON, RIGHT_COL_X + PADDING, TABLE_START_Y - 32);

    // Vertical separator between columns
    float separatorX = RIGHT_COL_X - (COL_GAP / 2.0f);
    DrawLine((int)separatorX, TABLE_START_Y - 24, (int)separatorX, TABLE_START_Y + TABLE_H + 8, LIGHTGRAY);

    // Prepare content heights
    const int incomeCount = incomes.getCount();
    const int expenseCount = expenses.getCount();
    float leftContentHeight = (float)((incomeCount + 1) * ROW_HEIGHT);
    float rightContentHeight = (float)((expenseCount + 1) * ROW_HEIGHT);

    scLeft.Update(leftContentHeight);
    scRight.Update(rightContentHeight);

    // --- LEFT: incomes table ---
    scLeft.Begin();
    {
        float y = scLeft.view.y - scLeft.GetOffset();

        // header
        LayoutHelper::MeasureAndDrawText("Date", 18, GRAY, scLeft.view.x + PADDING, y + 8);
        LayoutHelper::MeasureAndDrawText("Source / Wallet", 18, GRAY, scLeft.view.x + 160, y + 8);
        LayoutHelper::MeasureAndDrawText("Amount", 18, GRAY, scLeft.view.x + scLeft.view.width - 140, y + 8);
        y += ROW_HEIGHT;

        for (int i = 0; i < incomeCount; ++i) {
            IncomeTransaction it = incomes.getAt(i);
            Rectangle rowRect = { scLeft.view.x, y, scLeft.view.width - 14.0f, (float)ROW_HEIGHT - 4.0f };

            Vector2 mouse = GetMousePosition();
            if (CheckCollisionPointRec(mouse, rowRect)) {
                DrawRectangleRec(rowRect, Fade(SKYBLUE, 0.12f));
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    printf("Income row %d clicked\n", i);
                }
            }

            char dateBuf[64];
            snprintf(dateBuf, sizeof(dateBuf), "%02d/%02d/%04d", it.day.day, it.day.month, it.day.year);
            LayoutHelper::MeasureAndDrawText(dateBuf, 18, BLACK, scLeft.view.x + PADDING, y + 8);

            std::string srcName = sources.getSourceName(it.sourceID);
            std::string wName = dataManager.wallets_.getName(it.walletID);
            std::string combined = srcName + " / " + wName;
            LayoutHelper::MeasureAndDrawText(combined, 18, DARKGRAY, scLeft.view.x + 160, y + 8);

            std::string amt = FormatCurrency((long long)it.amount);
            int amtW = MeasureText(amt.c_str(), 18);
            LayoutHelper::MeasureAndDrawText(amt, 18, GREEN, scLeft.view.x + scLeft.view.width - 20 - amtW, y + 8);

            if (!it.description.empty()) {
                LayoutHelper::MeasureAndDrawText(it.description, 16, GRAY, scLeft.view.x + PADDING, y + 24);
            }

            y += ROW_HEIGHT;
        }
    }
    scLeft.End();

    // --- RIGHT: expenses table ---
    scRight.Begin();
    {
        float y = scRight.view.y - scRight.GetOffset();

        // header
        LayoutHelper::MeasureAndDrawText("Date", 18, GRAY, scRight.view.x + PADDING, y + 8);
        LayoutHelper::MeasureAndDrawText("Category / Wallet", 18, GRAY, scRight.view.x + 160, y + 8);
        LayoutHelper::MeasureAndDrawText("Amount", 18, GRAY, scRight.view.x + scRight.view.width - 140, y + 8);
        y += ROW_HEIGHT;

        for (int i = 0; i < expenseCount; ++i) {
            ExpenseTransaction et = expenses.getAt(i);
            Rectangle rowRect = { scRight.view.x, y, scRight.view.width - 14.0f, (float)ROW_HEIGHT - 4.0f };

            Vector2 mouse = GetMousePosition();
            if (CheckCollisionPointRec(mouse, rowRect)) {
                DrawRectangleRec(rowRect, Fade(SKYBLUE, 0.12f));
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    printf("Expense row %d clicked\n", i);
                }
            }

            char dateBuf[64];
            snprintf(dateBuf, sizeof(dateBuf), "%02d/%02d/%04d", et.day.day, et.day.month, et.day.year);
            LayoutHelper::MeasureAndDrawText(dateBuf, 18, BLACK, scRight.view.x + PADDING, y + 8);

            std::string catName = categories.getCategoryName(et.categoryID);
            std::string wName = dataManager.wallets_.getName(et.walletID);
            std::string combined = catName + " / " + wName;
            LayoutHelper::MeasureAndDrawText(combined, 18, DARKGRAY, scRight.view.x + 160, y + 8);

            std::string amt = FormatCurrency((long long)et.amount);
            int amtW = MeasureText(amt.c_str(), 18);
            LayoutHelper::MeasureAndDrawText(amt, 18, RED, scRight.view.x + scRight.view.width - 20 - amtW, y + 8);

            if (!et.description.empty()) {
                LayoutHelper::MeasureAndDrawText(et.description, 16, GRAY, scRight.view.x + PADDING, y + 24);
            }

            y += ROW_HEIGHT;
        }
    }
    scRight.End();

    if (showAddIncomeDialog) {
        DrawAddIncomeDialog();
    }
    if (showAddExpenseDialog) {
        DrawAddExpenseDialog();
    }
    if (showConfirmDialog) {
        DrawConfirmDialog();
    }

}

void TransactionWindow::PopulateWalletDropdown(Dropdown& dropdown) {
    if (!dataManagerRef) return;

    int count = 0;
    int* ids = dataManagerRef->wallets_.getAllIDs(count);

    if (!ids || count == 0) {
        dropdown.AddOption("No wallets", -1);
    } else {
        for (int i = 0; i < count; ++i) {
            int id = ids[i];
            dropdown.AddOption(dataManagerRef->wallets_.getName(id), id);
        }
    }

    delete[] ids;
}

void TransactionWindow::PopulateSourceDropdown(Dropdown& dropdown) {
    if (!dataManagerRef) return;

    int count = 0;
    int* ids = dataManagerRef->sources_.getAllIDs(count);

    if (!ids || count == 0) {
        dropdown.AddOption("No sources", -1);
    } else {
        for (int i = 0; i < count; ++i) {
            int id = ids[i];
            dropdown.AddOption(dataManagerRef->sources_.getSourceName(id), id);
        }
    }

    delete[] ids;
}

void TransactionWindow::PopulateCategoryDropdown(Dropdown& dropdown) {
    if (!dataManagerRef) return;

    int count = 0;
    int* ids = dataManagerRef->categories_.getAllIDs(count);

    if (!ids || count == 0) {
        dropdown.AddOption("No categories", -1);
    } else {
        for (int i = 0; i < count; ++i) {
            int id = ids[i];
            dropdown.AddOption(dataManagerRef->categories_.getCategoryName(id), id);
        }
    }

    delete[] ids;
}

void TransactionWindow::OpenIncomeDialog() {
    if (!dataManagerRef) return;

    showAddExpenseDialog = false;
    addExpenseDialog.Close();

    addIncomeDialog.ResetInputs();
    addIncomeDialog.Open();
    showAddIncomeDialog = true;
    confirmingIncome = true;
    errorID = -1;

    // Setup dropdowns
    incomeWalletDropdown = std::make_unique<Dropdown>(incomeDialogRect.x + dialogBoxes[0].x,
                                                      incomeDialogRect.y + dialogBoxes[0].y,
                                                      dialogBoxes[0].width, dialogBoxes[0].height, "Wallet");
    incomeSourceDropdown = std::make_unique<Dropdown>(incomeDialogRect.x + dialogBoxes[1].x,
                                                       incomeDialogRect.y + dialogBoxes[1].y,
                                                       dialogBoxes[1].width, dialogBoxes[1].height, "Income source");
    PopulateWalletDropdown(*incomeWalletDropdown);
    PopulateSourceDropdown(*incomeSourceDropdown);
}

void TransactionWindow::OpenExpenseDialog() {
    if (!dataManagerRef) return;

    showAddIncomeDialog = false;
    addIncomeDialog.Close();

    addExpenseDialog.ResetInputs();
    addExpenseDialog.Open();
    showAddExpenseDialog = true;
    confirmingIncome = false;
    errorID = -1;

    expenseWalletDropdown = std::make_unique<Dropdown>(expenseDialogRect.x + dialogBoxes[0].x,
                                                      expenseDialogRect.y + dialogBoxes[0].y,
                                                      dialogBoxes[0].width, dialogBoxes[0].height, "Wallet");
    expenseCategoryDropdown = std::make_unique<Dropdown>(expenseDialogRect.x + dialogBoxes[1].x,
                                                         expenseDialogRect.y + dialogBoxes[1].y,
                                                         dialogBoxes[1].width, dialogBoxes[1].height, "Category");
    PopulateWalletDropdown(*expenseWalletDropdown);
    PopulateCategoryDropdown(*expenseCategoryDropdown);
}

void TransactionWindow::SubmitIncome() {
    if (!dataManagerRef || !incomeWalletDropdown || !incomeSourceDropdown) return;

    std::string amtText = addIncomeDialog.GetInputText(INCOME_AMOUNT_IDX);
    std::string desc = addIncomeDialog.GetInputText(INCOME_DESC_IDX);

    int walletId = incomeWalletDropdown->GetSelectedValue();
    int sourceId = incomeSourceDropdown->GetSelectedValue();
    double amount = 0.0;

    try {
        amount = std::stod(amtText);
    } catch (...) {
        errorID = 1; // amount invalid
        return;
    }

    if (walletId < 0) { errorID = 4; return; } // wallet not selected
    if (sourceId < 0) { errorID = 5; return; } // source not selected

    date today = getCurrentDate();
    dataManagerRef->incomes_.addTransaction(today, amount, sourceId, walletId, desc);
    dataManagerRef->SaveAllData();

    addIncomeDialog.ResetInputs();
    addIncomeDialog.Close();
    showAddIncomeDialog = false;
    confirmingIncome = true;
    errorID = -1;
    ShowConfirm("Income added.");
}

void TransactionWindow::SubmitExpense() {
    if (!dataManagerRef || !expenseWalletDropdown || !expenseCategoryDropdown) return;

    std::string amtText = addExpenseDialog.GetInputText(EXPENSE_AMOUNT_IDX);
    std::string desc = addExpenseDialog.GetInputText(EXPENSE_DESC_IDX);

    int walletId = expenseWalletDropdown->GetSelectedValue();
    int categoryId = expenseCategoryDropdown->GetSelectedValue();
    double amount = 0.0;

    try {
        amount = std::stod(amtText);
    } catch (...) {
        errorID = 1; // amount invalid
        return;
    }

    if (walletId < 0) { errorID = 4; return; } // wallet not selected
    if (categoryId < 0) { errorID = 6; return; } // category not selected

    date today = getCurrentDate();
    dataManagerRef->expenses_.addTransaction(today, amount, categoryId, walletId, desc);
    dataManagerRef->SaveAllData();

    addExpenseDialog.ResetInputs();
    addExpenseDialog.Close();
    showAddExpenseDialog = false;
    confirmingIncome = false;
    errorID = -1;
    ShowConfirm("Expense added.");
}

void TransactionWindow::CloseDialogs() {
    showAddIncomeDialog = false;
    showAddExpenseDialog = false;
    addIncomeDialog.Close();
    addExpenseDialog.Close();
    errorID = -1;
}

void TransactionWindow::ShowConfirm(const std::string& message) {
    confirmMessage = message;
    showConfirmDialog = true;
    confirmDialog.Open();
}

void TransactionWindow::DrawAddIncomeDialog() {
    addIncomeDialog.Draw();
    // Labels sit above each input, using the shared grid positions
    int walletLabelY = (int)(incomeDialogRect.y + dialogBoxes[0].y - 26);
    int sourceLabelY = (int)(incomeDialogRect.y + dialogBoxes[1].y - 26);
    int amountLabelY = (int)(incomeDialogRect.y + dialogBoxes[2].y - 26);
    int descLabelY = (int)(incomeDialogRect.y + dialogBoxes[3].y - 26);

    DrawText("Wallet", (int)(incomeDialogRect.x + dialogBoxes[0].x), walletLabelY, 20, DARKGRAY);
    if (incomeWalletDropdown) incomeWalletDropdown->DrawBase();

    DrawText("Income source", (int)(incomeDialogRect.x + dialogBoxes[1].x), sourceLabelY, 20, DARKGRAY);
    if (incomeSourceDropdown) incomeSourceDropdown->DrawBase();

    DrawText("Amount", (int)(incomeDialogRect.x + dialogBoxes[2].x), amountLabelY, 20, DARKGRAY);
    DrawText("Description", (int)(incomeDialogRect.x + dialogBoxes[3].x), descLabelY, 20, DARKGRAY);

    // Draw dropdown overlays last so they sit above labels/fields
    if (incomeWalletDropdown) incomeWalletDropdown->DrawListOverlay();
    if (incomeSourceDropdown) incomeSourceDropdown->DrawListOverlay();

    // Error indicator
    DrawFormErrorTextIndicator(addIncomeDialog.GetRect(), errorID);
}

void TransactionWindow::DrawAddExpenseDialog() {
    addExpenseDialog.Draw();

    int walletLabelY = (int)(expenseDialogRect.y + dialogBoxes[0].y - 26);
    int categoryLabelY = (int)(expenseDialogRect.y + dialogBoxes[1].y - 26);
    int amountLabelY = (int)(expenseDialogRect.y + dialogBoxes[2].y - 26);
    int descLabelY = (int)(expenseDialogRect.y + dialogBoxes[3].y - 26);

    DrawText("Wallet", (int)(expenseDialogRect.x + dialogBoxes[0].x), walletLabelY, 20, DARKGRAY);
    if (expenseWalletDropdown) expenseWalletDropdown->DrawBase();

    DrawText("Category", (int)(expenseDialogRect.x + dialogBoxes[1].x), categoryLabelY, 20, DARKGRAY);
    if (expenseCategoryDropdown) expenseCategoryDropdown->DrawBase();

    DrawText("Amount", (int)(expenseDialogRect.x + dialogBoxes[2].x), amountLabelY, 20, DARKGRAY);
    DrawText("Description", (int)(expenseDialogRect.x + dialogBoxes[3].x), descLabelY, 20, DARKGRAY);

    if (expenseWalletDropdown) expenseWalletDropdown->DrawListOverlay();
    if (expenseCategoryDropdown) expenseCategoryDropdown->DrawListOverlay();

    // Error indicator
    DrawFormErrorTextIndicator(addExpenseDialog.GetRect(), errorID);
}   

void TransactionWindow::DrawConfirmDialog() {
    confirmDialog.Draw();
    DrawText(confirmMessage.c_str(), (int)(confirmDialogRect.x + 24), (int)(confirmDialogRect.y + 60), 20, BLACK);
}

