#include "RecurringWindow.h"
#include "Helper.h"

#include <cstdio>
#include <string>

// Layout constants tuned for 1280x800 window
static const int screenWidth = 1280;
static const int screenHeight = 800;
static const int PADDING = 24;
static const int TITLE_H = 72;
static const int CTRL_W = 230;
static const int CTRL_H = 46;
static const int TABLE_TOP = 150;
static const int TABLE_H = 520;
static const int ROW_H = 40;

RecurringWindow::RecurringWindow() {}
RecurringWindow::~RecurringWindow() {
    delete[] inputGrid_;
}

void RecurringWindow::Init() {
    if (initialized_) return;

    listArea_.Init((float)PADDING, (float)TABLE_TOP, (float)(screenWidth - PADDING * 2), (float)TABLE_H);

    backBtn_ = Button((float)PADDING, 18.0f, 140.0f, (float)CTRL_H, "< Dashboard",
                      LIGHTGRAY, GRAY, BLACK, [this]() { if (onBackRequested_) onBackRequested_(); }, 18);

    addIncomeBtn_ = Button((float)(screenWidth - PADDING - CTRL_W * 2 - 10), 18.0f, (float)CTRL_W, (float)CTRL_H,
                           "Add Recurring Income", GREEN, DARKGREEN, WHITE, [this]() { OpenIncomeDialog(); }, 18);

    addExpenseBtn_ = Button((float)(screenWidth - PADDING - CTRL_W), 18.0f, (float)CTRL_W, (float)CTRL_H,
                            "Add Recurring Expense", GREEN, DARKGREEN, WHITE, [this]() { OpenExpenseDialog(); }, 18);

    dialogRect_ = LayoutHelper::CenterRect(0, 0, (float)screenWidth, (float)screenHeight, 900.0f, 520.0f);

    // Build grid: 3 rows x 2 cols -> row1 wallet/source, row2 amount/description, row3 start/end
    delete[] inputGrid_;
    inputGrid_ = LayoutHelper::CreateGrid(40, 130, 3, 2, 320.0f, 44.0f, 28.0f);
    inputCount_ = 6;

    addDialog_ = Dialog(dialogRect_.x, dialogRect_.y, dialogRect_.width, dialogRect_.height, "Recurring Transaction");
    addDialog_.AddTextInput(inputGrid_[2].x, inputGrid_[2].y, inputGrid_[2].width, inputGrid_[2].height, "Amount");
    addDialog_.AddTextInput(inputGrid_[3].x, inputGrid_[3].y, inputGrid_[3].width, inputGrid_[3].height, "Description");
    addDialog_.AddTextInput(inputGrid_[4].x, inputGrid_[4].y, inputGrid_[4].width, inputGrid_[4].height, "Start date (DD/MM/YYYY)");
    addDialog_.AddTextInput(inputGrid_[5].x, inputGrid_[5].y, inputGrid_[5].width, inputGrid_[5].height, "End date (optional)");
    // Character caps
    addDialog_.SetInputMaxLength(INPUT_AMOUNT_IDX, 20);
    addDialog_.SetInputMaxLength(INPUT_DESC_IDX, 120);
    addDialog_.SetInputMaxLength(INPUT_START_IDX, 10);
    addDialog_.SetInputMaxLength(INPUT_END_IDX, 10);
    addDialog_.AddButton(dialogRect_.width - 220, dialogRect_.height - 70, 100, 46, "Add",
                         GREEN, DARKGREEN, WHITE, [this]() { SubmitRecurring(); });
    addDialog_.AddButton(dialogRect_.width - 110, dialogRect_.height - 70, 100, 46, "Cancel",
                         RED, MAROON, WHITE, [this]() { CloseDialog(); });

    // Confirm dialog (shared for income/expense)
    Rectangle confirmRect = LayoutHelper::CenterRect(0, 0, (float)screenWidth, (float)screenHeight, 420.0f, 220.0f);
    confirmDialog_ = Dialog(confirmRect.x, confirmRect.y, confirmRect.width, confirmRect.height, "Saved");
    confirmDialog_.AddButton(24, confirmRect.height - 70, 160, 44, "Keep adding",
                             GREEN, DARKGREEN, WHITE, [this]() {
                                 showConfirmDialog_ = false;
                                 confirmDialog_.Close();
                                 if (lastWasIncome_) OpenIncomeDialog(); else OpenExpenseDialog();
                             });
    confirmDialog_.AddButton(confirmRect.width - 184, confirmRect.height - 70, 160, 44, "Dashboard",
                             SKYBLUE, BLUE, WHITE, [this]() {
                                 showConfirmDialog_ = false;
                                 confirmDialog_.Close();
                                 if (onBackRequested_) onBackRequested_();
                             });

    initialized_ = true;
}

void RecurringWindow::Update() {
    if (!initialized_) Init();

    // Only update buttons and list if no dialog is open
    if (!showAddDialog_ && !showConfirmDialog_) {
        backBtn_.Update();
        addIncomeBtn_.Update();
        addExpenseBtn_.Update();
    }

    if (showAddDialog_) {
        addDialog_.Update();
        if (walletDropdown_) walletDropdown_->Update();
        if (addingIncome_ && sourceDropdown_) sourceDropdown_->Update();
        if (!addingIncome_ && categoryDropdown_) categoryDropdown_->Update();
    }

    if (showConfirmDialog_) {
        confirmDialog_.Update();
    }
}

void RecurringWindow::Draw(DataManager& dataManager) {
    if (!initialized_) Init();
    dataManager_ = &dataManager;

    // Top bar
    Rectangle titleBox = { (float)PADDING, 10.0f, (float)(screenWidth - PADDING * 2), (float)TITLE_H };
    DrawRectangleRec(titleBox, Fade(SKYBLUE, 0.10f));
    DrawRectangleLinesEx(titleBox, 2.0f, DARKBLUE);
    LayoutHelper::MeasureAndDrawText("Recurring", 32, DARKBLUE, titleBox.x + PADDING + 150 , titleBox.y + 18);

    backBtn_.Draw();
    addIncomeBtn_.Draw();
    addExpenseBtn_.Draw();

    DrawList(dataManager);

    if (showAddDialog_) {
        DrawAddDialog();
    }

    if (showConfirmDialog_) {
        DrawConfirmDialog();
    }
}

void RecurringWindow::DrawList(DataManager& dataManager) {
    RecurringManager& rm = dataManager.recurring_;
    int count = rm.getCount();

    float contentH = (float)((count + 1) * ROW_H);
    listArea_.Update(contentH);

    // headers
    LayoutHelper::MeasureAndDrawText("Type", 20, DARKGRAY, listArea_.view.x + 12, listArea_.view.y - 30);
    LayoutHelper::MeasureAndDrawText("Wallet", 20, DARKGRAY, listArea_.view.x + 120, listArea_.view.y - 30);
    LayoutHelper::MeasureAndDrawText("Source / Category", 20, DARKGRAY, listArea_.view.x + 360, listArea_.view.y - 30);
    LayoutHelper::MeasureAndDrawText("Amount", 20, DARKGRAY, listArea_.view.x + listArea_.view.width - 360, listArea_.view.y - 30);
    LayoutHelper::MeasureAndDrawText("Start -> End", 20, DARKGRAY, listArea_.view.x + listArea_.view.width - 240, listArea_.view.y - 30);

    listArea_.Begin();
    float y = listArea_.view.y - listArea_.GetOffset();

    for (int i = 0; i < count; ++i) {
        RecurringTask task = rm.getAt(i);
        Rectangle row = { listArea_.view.x, y, listArea_.view.width - 12.0f, (float)ROW_H - 4.0f };

        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, row)) {
            DrawRectangleRec(row, Fade(SKYBLUE, 0.10f));
        }

        const char* typeText = task.isIncome ? "Income" : "Expense";
        LayoutHelper::MeasureAndDrawText(typeText, 18, task.isIncome ? DARKGREEN : MAROON, row.x + 12, y + 10);

        std::string walletName = dataManager.wallets_.getName(task.walletID);
        LayoutHelper::MeasureAndDrawText(walletName, 18, BLACK, row.x + 120, y + 10);

        std::string srcCat = task.isIncome ? dataManager.sources_.getSourceName(task.categoryOrSourceID)
                                           : dataManager.categories_.getCategoryName(task.categoryOrSourceID);
        LayoutHelper::MeasureAndDrawText(srcCat, 18, DARKGRAY, row.x + 360, y + 10);

        std::string amt = FormatCurrency((long long)task.amount);
        int amtW = MeasureText(amt.c_str(), 18);
        LayoutHelper::MeasureAndDrawText(amt, 18, task.isIncome ? GREEN : RED, row.x + row.width - 260 - amtW, y + 10);

        std::string dateStr = FormatDate(task.startDate) + " -> " + FormatDate(task.endDate);
        LayoutHelper::MeasureAndDrawText(dateStr, 18, DARKGRAY, row.x + row.width - 240, y + 10);

        if (!task.description.empty()) {
            LayoutHelper::MeasureAndDrawText(task.description, 16, GRAY, row.x + 12, y + 28);
        }

        y += ROW_H;
    }

    listArea_.End();
}

void RecurringWindow::OpenIncomeDialog() {
    if (!dataManager_) return;
    addingIncome_ = true;
    lastWasIncome_ = true;
    showAddDialog_ = true;
    errorID_ = -1;
    addDialog_.ResetInputs();
    addDialog_.Open();

    // Setup dropdowns
    walletDropdown_ = std::make_unique<Dropdown>(dialogRect_.x + inputGrid_[0].x, dialogRect_.y + inputGrid_[0].y,
                                                 inputGrid_[0].width, inputGrid_[0].height, "Wallet");
    sourceDropdown_ = std::make_unique<Dropdown>(dialogRect_.x + inputGrid_[1].x, dialogRect_.y + inputGrid_[1].y,
                                                 inputGrid_[1].width, inputGrid_[1].height, "Income source");
    categoryDropdown_.reset();
    PopulateWalletDropdown(*walletDropdown_);
    PopulateSourceDropdown(*sourceDropdown_);
}

void RecurringWindow::OpenExpenseDialog() {
    if (!dataManager_) return;
    addingIncome_ = false;
    lastWasIncome_ = false;
    showAddDialog_ = true;
    errorID_ = -1;
    addDialog_.ResetInputs();
    addDialog_.Open();

    // Setup dropdowns
    walletDropdown_ = std::make_unique<Dropdown>(dialogRect_.x + inputGrid_[0].x, dialogRect_.y + inputGrid_[0].y,
                                                 inputGrid_[0].width, inputGrid_[0].height, "Wallet");
    categoryDropdown_ = std::make_unique<Dropdown>(dialogRect_.x + inputGrid_[1].x, dialogRect_.y + inputGrid_[1].y,
                                                   inputGrid_[1].width, inputGrid_[1].height, "Category");
    sourceDropdown_.reset();
    PopulateWalletDropdown(*walletDropdown_);
    PopulateCategoryDropdown(*categoryDropdown_);
}

void RecurringWindow::CloseDialog() {
    showAddDialog_ = false;
    errorID_ = -1;
    addDialog_.Close();
}

void RecurringWindow::ShowConfirm(const std::string& message) {
    confirmMessage_ = message;
    showConfirmDialog_ = true;
    confirmDialog_.Open();
}

void RecurringWindow::DrawConfirmDialog() {
    confirmDialog_.Draw();
    Rectangle r = confirmDialog_.GetRect();
    DrawText(confirmMessage_.c_str(), (int)(r.x + 24), (int)(r.y + 64), 20, BLACK);
}



void RecurringWindow::SubmitRecurring() {
    if (!dataManager_ || !walletDropdown_) return;

    errorID_ = -1;

    std::string amtText = addDialog_.GetInputText(INPUT_AMOUNT_IDX);
    std::string desc = addDialog_.GetInputText(INPUT_DESC_IDX);
    std::string startText = addDialog_.GetInputText(INPUT_START_IDX);
    std::string endText = addDialog_.GetInputText(INPUT_END_IDX);

    long long amt = 0;
    if (!IsPositiveMoneyLLong(amtText, amt)) { errorID_ = 1; return; }

    date start{};
    if (!ParseDateString(startText, start)) { errorID_ = 2; return; }

    date end{};
    if (!endText.empty() && !ParseDateString(endText, end)) { errorID_ = 3; return; }

    int walletId = walletDropdown_->GetSelectedValue();
    if (walletId < 0) { errorID_ = 4; return; }

    int catSrcId = -1;
    if (addingIncome_) {
        if (!sourceDropdown_) { errorID_ = 5; return; }
        catSrcId = sourceDropdown_->GetSelectedValue();
        if (catSrcId < 0) { errorID_ = 5; return; }
    } else {
        if (!categoryDropdown_) { errorID_ = 6; return; }
        catSrcId = categoryDropdown_->GetSelectedValue();
        if (catSrcId < 0) { errorID_ = 6; return; }
    }

    dataManager_->recurring_.addRecurring(addingIncome_, static_cast<double>(amt), catSrcId, walletId, desc, start, end);
    dataManager_->SaveAllData();

    errorID_ = -1;
    CloseDialog();
    ShowConfirm(addingIncome_ ? "Recurring income saved." : "Recurring expense saved.");
}

void RecurringWindow::PopulateWalletDropdown(Dropdown& dropdown) {
    int count = 0;
    int* ids = dataManager_->wallets_.getAllIDs(count);
    if (!ids || count == 0) {
        dropdown.AddOption("No wallets", -1);
    } else {
        for (int i = 0; i < count; ++i) {
            int id = ids[i];
            dropdown.AddOption(dataManager_->wallets_.getName(id), id);
        }
    }
    delete[] ids;
}

void RecurringWindow::PopulateSourceDropdown(Dropdown& dropdown) {
    int count = 0;
    int* ids = dataManager_->sources_.getAllIDs(count);
    if (!ids || count == 0) {
        dropdown.AddOption("No sources", -1);
    } else {
        for (int i = 0; i < count; ++i) {
            int id = ids[i];
            dropdown.AddOption(dataManager_->sources_.getSourceName(id), id);
        }
    }
    delete[] ids;
}

void RecurringWindow::PopulateCategoryDropdown(Dropdown& dropdown) {
    int count = 0;
    int* ids = dataManager_->categories_.getAllIDs(count);
    if (!ids || count == 0) {
        dropdown.AddOption("No categories", -1);
    } else {
        for (int i = 0; i < count; ++i) {
            int id = ids[i];
            dropdown.AddOption(dataManager_->categories_.getCategoryName(id), id);
        }
    }
    delete[] ids;
}

std::string RecurringWindow::FormatDate(const date& d) const {
    if (d.year == 0) return "No end";
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%02d/%02d/%04d", d.day, d.month, d.year);
    return std::string(buf);
}

// Draw the add/edit dialog with all controls
void RecurringWindow::DrawAddDialog() {
    addDialog_.Draw();

    if (!inputGrid_) return;

    int walletLabelY = (int)(dialogRect_.y + inputGrid_[0].y - 26);
    int sourceLabelY = (int)(dialogRect_.y + inputGrid_[1].y - 26);
    int amountLabelY = (int)(dialogRect_.y + inputGrid_[2].y - 26);
    int descLabelY = (int)(dialogRect_.y + inputGrid_[3].y - 26);
    int startLabelY = (int)(dialogRect_.y + inputGrid_[4].y - 26);
    int endLabelY = (int)(dialogRect_.y + inputGrid_[5].y - 26);

    DrawText("Wallet", (int)(dialogRect_.x + inputGrid_[0].x), walletLabelY, 20, DARKGRAY);
    if (walletDropdown_) walletDropdown_->DrawBase();

    if (addingIncome_) {
        DrawText("Income source", (int)(dialogRect_.x + inputGrid_[1].x), sourceLabelY, 20, DARKGRAY);
        if (sourceDropdown_) sourceDropdown_->DrawBase();
    } else {
        DrawText("Category", (int)(dialogRect_.x + inputGrid_[1].x), sourceLabelY, 20, DARKGRAY);
        if (categoryDropdown_) categoryDropdown_->DrawBase();
    }

    DrawText("Amount", (int)(dialogRect_.x + inputGrid_[2].x), amountLabelY, 20, DARKGRAY);
    DrawText("Description", (int)(dialogRect_.x + inputGrid_[3].x), descLabelY, 20, DARKGRAY);
    DrawText("Start date", (int)(dialogRect_.x + inputGrid_[4].x), startLabelY, 20, DARKGRAY);
    DrawText("End date", (int)(dialogRect_.x + inputGrid_[5].x), endLabelY, 20, DARKGRAY);

    // Overlay lists last to ensure they are above labels
    if (walletDropdown_) walletDropdown_->DrawListOverlay();
    if (addingIncome_) {
        if (sourceDropdown_) sourceDropdown_->DrawListOverlay();
    } else {
        if (categoryDropdown_) categoryDropdown_->DrawListOverlay();
    }


    Rectangle AddButtonRect = { dialogRect_.x + dialogRect_.width - 220.0f, dialogRect_.y + dialogRect_.height - 70.0f, 100.0f, 46.0f };
    DrawFormErrorTextIndicator(AddButtonRect, errorID_);
}
