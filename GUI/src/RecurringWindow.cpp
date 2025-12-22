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
    addDialog_.AddButton(dialogRect_.width - 220, dialogRect_.height - 70, 100, 46, "Add",
                         GREEN, DARKGREEN, WHITE, [this]() { SubmitRecurring(); });
    addDialog_.AddButton(dialogRect_.width - 110, dialogRect_.height - 70, 100, 46, "Cancel",
                         RED, MAROON, WHITE, [this]() { CloseDialog(); });

    initialized_ = true;
}

void RecurringWindow::Update() {
    if (!initialized_) Init();

    backBtn_.Update();
    addIncomeBtn_.Update();
    addExpenseBtn_.Update();

    if (showAddDialog_) {
        addDialog_.Update();
        if (walletDropdown_) walletDropdown_->Update();
        if (addingIncome_ && sourceDropdown_) sourceDropdown_->Update();
        if (!addingIncome_ && categoryDropdown_) categoryDropdown_->Update();
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

    if (!statusMessage_.empty()) {
        DrawText(statusMessage_.c_str(), (int)(titleBox.x + PADDING), (int)(titleBox.y + TITLE_H - 18), 18, statusColor_);
    }

    DrawList(dataManager);

    if (showAddDialog_) {
        DrawAddDialog();
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
    LayoutHelper::MeasureAndDrawText("Amount", 20, DARKGRAY, listArea_.view.x + listArea_.view.width - 240, listArea_.view.y - 30);
    LayoutHelper::MeasureAndDrawText("Start -> End", 20, DARKGRAY, listArea_.view.x + listArea_.view.width - 120, listArea_.view.y - 30);

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
        LayoutHelper::MeasureAndDrawText(dateStr, 18, DARKGRAY, row.x + row.width - 120, y + 10);

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
    showAddDialog_ = true;
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
    showAddDialog_ = true;
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
    addDialog_.Close();
}

bool RecurringWindow::ParseDate(const std::string& text, date& out, bool allowEmpty) {
    if (text.empty()) {
        if (allowEmpty) { out = {0,0,0}; return true; }
        return false;
    }
    int d = 0, m = 0, y = 0;

    // Get day, month, year by scanning
    if (std::sscanf(text.c_str(), "%d/%d/%d", &d, &m, &y) != 3) return false;
    if (d <= 0 || m <= 0 || m > 12 || y <= 0) return false;
    out = {d, m, y};
    return true;
}

void RecurringWindow::SubmitRecurring() {
    if (!dataManager_ || !walletDropdown_) return;

    std::string amtText = addDialog_.GetInputText(INPUT_AMOUNT_IDX);
    std::string desc = addDialog_.GetInputText(INPUT_DESC_IDX);
    std::string startText = addDialog_.GetInputText(INPUT_START_IDX);
    std::string endText = addDialog_.GetInputText(INPUT_END_IDX);

    double amount = 0.0;
    try { amount = std::stod(amtText); } catch (...) { statusMessage_ = "Invalid amount"; statusColor_ = RED; return; }
    if (amount <= 0) { statusMessage_ = "Amount must be > 0"; statusColor_ = RED; return; }

    date start{};
    if (!ParseDate(startText, start, false)) { statusMessage_ = "Invalid start date"; statusColor_ = RED; return; }

    date end{};
    if (!ParseDate(endText, end, true)) { statusMessage_ = "Invalid end date"; statusColor_ = RED; return; }

    int walletId = walletDropdown_->GetSelectedValue();
    if (walletId < 0) { statusMessage_ = "Select a wallet"; statusColor_ = RED; return; }

    int catSrcId = -1;
    if (addingIncome_) {
        if (!sourceDropdown_) return;
        catSrcId = sourceDropdown_->GetSelectedValue();
        if (catSrcId < 0) { statusMessage_ = "Select a source"; statusColor_ = RED; return; }
    } else {
        if (!categoryDropdown_) return;
        catSrcId = categoryDropdown_->GetSelectedValue();
        if (catSrcId < 0) { statusMessage_ = "Select a category"; statusColor_ = RED; return; }
    }

    dataManager_->recurring_.addRecurring(addingIncome_, amount, catSrcId, walletId, desc, start, end);
    dataManager_->SaveAllData();

    statusMessage_ = addingIncome_ ? "Recurring income added" : "Recurring expense added";
    statusColor_ = DARKGREEN;

    CloseDialog();
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
    if (walletDropdown_) walletDropdown_->Draw();

    if (addingIncome_) {
        DrawText("Income source", (int)(dialogRect_.x + inputGrid_[1].x), sourceLabelY, 20, DARKGRAY);
        if (sourceDropdown_) sourceDropdown_->Draw();
    } else {
        DrawText("Category", (int)(dialogRect_.x + inputGrid_[1].x), sourceLabelY, 20, DARKGRAY);
        if (categoryDropdown_) categoryDropdown_->Draw();
    }

    DrawText("Amount", (int)(dialogRect_.x + inputGrid_[2].x), amountLabelY, 20, DARKGRAY);
    DrawText("Description", (int)(dialogRect_.x + inputGrid_[3].x), descLabelY, 20, DARKGRAY);
    DrawText("Start date", (int)(dialogRect_.x + inputGrid_[4].x), startLabelY, 20, DARKGRAY);
    DrawText("End date", (int)(dialogRect_.x + inputGrid_[5].x), endLabelY, 20, DARKGRAY);
}
