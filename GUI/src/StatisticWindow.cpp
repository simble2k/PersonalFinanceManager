#include "StatisticWindow.h"
#include "Helper.h"

// Layout constants
static const int PADDING = 60;
static const int LEFT_COLUMN_WIDTH = 900;
static const int RIGHT_COLUMN_START_X = STATS_SCREEN_W - 320;
static const int BTN_W = 280;
static const int BTN_H = 50;
static const int BTN_SPACING = 15;
static const int LINE_H = 24;

StatisticWindow::StatisticWindow() {}
StatisticWindow::~StatisticWindow() { 
    if (reportLines_) {
        delete[] reportLines_;
        reportLines_ = nullptr;
    }
    if (walletDropdown_) {
        delete walletDropdown_;
        walletDropdown_ = nullptr;
    }
}

void StatisticWindow::Init() {
    if (initialized_) return;

    // Allocate dynamic report lines array
    reportLineCapacity_ = 512;
    reportLines_ = new std::string[reportLineCapacity_];
    reportLineCount_ = 0;

    // Back button (top-left)
    backBtn_ = Button((float)PADDING, 20.0f, 140.0f, BTN_H, "< Dashboard", LIGHTGRAY, GRAY, BLACK,
                      [this]() { if (onBackRequested_) onBackRequested_(); });

    // Right column buttons (vertical stack)
    const int NUM_BUTTONS = 6;
    Rectangle* buttonRects = LayoutHelper::CreateGrid(
        RIGHT_COLUMN_START_X, PADDING + 80, NUM_BUTTONS, 1, BTN_W, BTN_H, BTN_SPACING);

    timeBasedBtn_ = Button(buttonRects[0].x, buttonRects[0].y, BTN_W, BTN_H, 
                           "1. Time-based", GREEN, DARKGREEN, WHITE,
                           [this]() { OpenDateRangeDialog(); });
    
    walletBasedBtn_ = Button(buttonRects[1].x, buttonRects[1].y, BTN_W, BTN_H,
                             "2. Wallet-based", BLUE, DARKBLUE, WHITE,
                             [this]() { ShowWalletBasedStats(); });
    
    annualOverviewBtn_ = Button(buttonRects[2].x, buttonRects[2].y, BTN_W, BTN_H,
                                "3. Annual Overview", SKYBLUE, BLUE, WHITE,
                                [this]() { ShowAnnualOverview(); });
    
    incomeBreakdownBtn_ = Button(buttonRects[3].x, buttonRects[3].y, BTN_W, BTN_H,
                                 "4. Income Breakdown", ORANGE, DARKGRAY, BLACK,
                                 [this]() { ShowIncomeBreakdown(); });
    
    expenseBreakdownBtn_ = Button(buttonRects[4].x, buttonRects[4].y, BTN_W, BTN_H,
                                  "5. Expense Breakdown", PURPLE, DARKPURPLE, WHITE,
                                  [this]() { ShowExpenseBreakdown(); });
    
    walletBalanceBtn_ = Button(buttonRects[5].x, buttonRects[5].y, BTN_W, BTN_H,
                               "6. Wallet Balance", YELLOW, GOLD, BLACK,
                               [this]() { OpenWalletSelectDialog(); });

    delete[] buttonRects;

    // Initialize date range dialog
    dateRangeDialog_ = Dialog(STATS_SCREEN_W / 2.0f - 200.0f, STATS_SCREEN_H / 2.0f - 150.0f, 400.0f, 300.0f, "Enter Date Range");
    dateRangeDialog_.AddTextInput(20, 60, 360, 40, "Start Date (DD/MM/YYYY)");
    dateRangeDialog_.AddTextInput(20, 120, 360, 40, "End Date (DD/MM/YYYY)");
    dateRangeDialog_.AddButton(60, 200, 120, 40, "OK", BLUE, DARKBLUE, WHITE,
                               [this]() { SubmitDateRange(); });
    dateRangeDialog_.AddButton(220, 200, 120, 40, "Cancel", RED, MAROON, WHITE,
                               [this]() { CloseDialogs(); });

    // Initialize wallet select dialog
    walletSelectDialog_ = Dialog(STATS_SCREEN_W / 2.0f - 200.0f, STATS_SCREEN_H / 2.0f - 100.0f, 400.0f, 250.0f, "Select Wallet");
    walletDropdown_ = new Dropdown(20, 60, 360, 40, "Select Wallet ID");
    walletSelectDialog_.AddButton(60, 140, 120, 40, "OK", BLUE, DARKBLUE, WHITE,
                                  [this]() { SubmitWalletSelect(); });
    walletSelectDialog_.AddButton(220, 140, 120, 40, "Cancel", RED, MAROON, WHITE,
                                  [this]() { CloseDialogs(); });

    // Results area on left
    resultsArea_.Init((float)PADDING, PADDING + 80.0f, (float)(LEFT_COLUMN_WIDTH - 20), 
                      (float)(STATS_SCREEN_H - PADDING * 2 - 80));

    // Column separator
    DrawLine(LEFT_COLUMN_WIDTH, PADDING, LEFT_COLUMN_WIDTH, STATS_SCREEN_H - PADDING, LIGHTGRAY);

    initialized_ = true;
}


void StatisticWindow::Update() {
    // Update buttons
    backBtn_.Update();
    timeBasedBtn_.Update();
    walletBasedBtn_.Update();
    annualOverviewBtn_.Update();
    incomeBreakdownBtn_.Update();
    expenseBreakdownBtn_.Update();
    walletBalanceBtn_.Update();

    // Update dialogs
    if (showDateRangeDialog_) {
        dateRangeDialog_.Update();
    }
    if (showWalletSelectDialog_) {
        walletSelectDialog_.Update();
        if (walletDropdown_) {
            walletDropdown_->Update();
        }
    }

    // Update scroll area content height
    float contentH = (float)((reportLineCount_ > 0 ? reportLineCount_ : 1) * LINE_H);
    resultsArea_.Update(contentH);
}

void StatisticWindow::Draw(DataManager& dm) {
    // Draw title
    LayoutHelper::CenterText("REPORTS & STATISTICS", 36, DARKBLUE, STATS_SCREEN_W / 2.0f, 20.0f);

    // Draw column separator line
    DrawLine(LEFT_COLUMN_WIDTH, PADDING, LEFT_COLUMN_WIDTH, STATS_SCREEN_H - PADDING, LIGHTGRAY);

    // Draw back button
    backBtn_.Draw();

    // Draw right column buttons
    timeBasedBtn_.Draw();
    walletBasedBtn_.Draw();
    annualOverviewBtn_.Draw();
    incomeBreakdownBtn_.Draw();
    expenseBreakdownBtn_.Draw();
    walletBalanceBtn_.Draw();

    // Draw right column title
    DrawText("--- FEATURES ---", RIGHT_COLUMN_START_X + 20, PADDING + 20, 20, DARKBLUE);

    // Draw results area border and content
    DrawRectangleLinesEx(Rectangle{ (float)PADDING, PADDING + 80.0f, 
                                    (float)(LEFT_COLUMN_WIDTH - 20), 
                                    (float)(STATS_SCREEN_H - PADDING * 2 - 80) }, 2.0f, LIGHTGRAY);

    // Clip and draw report lines
    resultsArea_.Begin();
    float y = PADDING + 80.0f - resultsArea_.GetOffset() + 8.0f;
    for (int i = 0; i < reportLineCount_; ++i) {
        DrawText(reportLines_[i].c_str(), PADDING + 12, y, 18, BLACK);
        y += LINE_H;
    }
    resultsArea_.End();

    // Draw error indicator if needed
    if (errorID_ > 0) {
        DrawFormErrorTextIndicator(Rectangle{ (float)PADDING, PADDING + 80.0f, 
                                             (float)(LEFT_COLUMN_WIDTH - 20), 40.0f }, errorID_);
    }

    // Draw dialogs on top
    if (showDateRangeDialog_) {
        dateRangeDialog_.Draw();
    }
    if (showWalletSelectDialog_) {
        walletSelectDialog_.Draw();
        if (walletDropdown_) {
            walletDropdown_->DrawBase();
        }
    }

    // Draw dropdown overlay last
    if (showWalletSelectDialog_ && walletDropdown_ && walletDropdown_->IsOpen()) {
        walletDropdown_->DrawListOverlay();
    }
}


void StatisticWindow::ClearReport() { reportLineCount_ = 0; }
void StatisticWindow::AppendLine(const std::string& s) { 
    if (reportLineCount_ < reportLineCapacity_) reportLines_[reportLineCount_++] = s; 
}

void StatisticWindow::OpenDateRangeDialog() {
    dateRangeDialog_.ResetInputs();
    showDateRangeDialog_ = true;
    errorID_ = -1;
}

void StatisticWindow::SubmitDateRange() {
    std::string startStr = dateRangeDialog_.GetInputText(0);
    std::string endStr = dateRangeDialog_.GetInputText(1);

    date startDate, endDate;

    // Validate start date
    if (!ParseDateString(startStr, startDate)) {
        errorID_ = 2; // Error ID 2 for start date
        return;
    }

    // Validate end date
    if (!ParseDateString(endStr, endDate)) {
        errorID_ = 3; // Error ID 3 for end date
        return;
    }

    // Both dates are valid, proceed
    errorID_ = -1;
    ShowTimeBasedStats(startDate, endDate);
    CloseDialogs();
}

void StatisticWindow::OpenWalletSelectDialog() {
    if (!dataManager_) return;

    // Clear and repopulate dropdown with wallet options
    if (walletDropdown_) {
        delete walletDropdown_;
    }
    walletDropdown_ = new Dropdown(40, 70, 320, 40, "Select Wallet");

    // Add all wallets to dropdown
    int walletCount = 0;
    int* walletIds = dataManager_->wallets_.getAllIDs(walletCount);
    
    for (int i = 0; i < walletCount; i++) {
        int walletId = walletIds[i];
        std::string walletName = dataManager_->wallets_.getWalletName(walletId);
        walletDropdown_->AddOption(walletName, walletId);
    }

    delete[] walletIds;

    showWalletSelectDialog_ = true;
    errorID_ = -1;
}

void StatisticWindow::SubmitWalletSelect() {
    if (!walletDropdown_) return;

    int selectedWalletId = walletDropdown_->GetSelectedValue();
    if (selectedWalletId < 0) {
        errorID_ = 4; // Error ID 4 for no wallet selected
        return;
    }

    errorID_ = -1;
    ShowWalletBalance(selectedWalletId);
    CloseDialogs();
}

void StatisticWindow::CloseDialogs() {
    showDateRangeDialog_ = false;
    showWalletSelectDialog_ = false;
    dateRangeDialog_.Close();
    walletSelectDialog_.Close();
}

static bool lessOrEq(date a, date b) { return compareDate(a, b) <= 0; }
static bool greaterOrEq(date a, date b) { return compareDate(a, b) >= 0; }

void StatisticWindow::ShowTimeBasedStats(date fromDate, date toDate) {
    if (!dataManager_) return;
    ClearReport();

    auto& incomes = dataManager_->incomes_;
    auto& expenses = dataManager_->expenses_;

    double totalInc = 0.0, totalExp = 0.0;
    for (int i = 0; i < incomes.getCount(); ++i) {
        IncomeTransaction t = incomes.getAt(i);
        if (greaterOrEq(t.day, fromDate) && lessOrEq(t.day, toDate)) totalInc += t.amount;
    }
    for (int i = 0; i < expenses.getCount(); ++i) {
        ExpenseTransaction t = expenses.getAt(i);
        if (greaterOrEq(t.day, fromDate) && lessOrEq(t.day, toDate)) totalExp += t.amount;
    }

    char fromBuf[16], toBuf[16];
    snprintf(fromBuf, sizeof(fromBuf), "%02d/%02d/%04d", fromDate.day, fromDate.month, fromDate.year);
    snprintf(toBuf, sizeof(toBuf), "%02d/%02d/%04d", toDate.day, toDate.month, toDate.year);

    AppendLine("--- TIME-BASED SUMMARY ---");
    AppendLine("");
    AppendLine(std::string("Period: ") + fromBuf + " to " + toBuf);
    AppendLine("");
    std::ostringstream oss;
    oss << "Total Income:  " << FormatCurrency((long long)totalInc); AppendLine(oss.str()); oss.str(""); oss.clear();
    oss << "Total Expense: " << FormatCurrency((long long)totalExp); AppendLine(oss.str()); oss.str(""); oss.clear();
    oss << "Net Balance:   " << FormatCurrency((long long)(totalInc - totalExp)); AppendLine(oss.str());
}

void StatisticWindow::ShowWalletBasedStats() {
    if (!dataManager_) return;
    ClearReport();

    auto& incomes = dataManager_->incomes_;
    auto& expenses = dataManager_->expenses_;
    auto& wallets = dataManager_->wallets_;

    // Aggregate per wallet (dynamic array, max 100 wallets)
    struct WalletAgg { int id; double inc, exp; };
    WalletAgg* agg = new WalletAgg[100];
    int aggCount = 0;
    auto touch = [&](int id) -> WalletAgg& {
        for (int i = 0; i < aggCount; ++i) if (agg[i].id == id) return agg[i];
        if (aggCount < 100) { agg[aggCount].id = id; agg[aggCount].inc = 0; agg[aggCount].exp = 0; return agg[aggCount++]; }
        return agg[0];
    };

    for (int i = 0; i < incomes.getCount(); ++i) {
        auto t = incomes.getAt(i);
        touch(t.walletID).inc += t.amount;
    }
    for (int i = 0; i < expenses.getCount(); ++i) {
        auto t = expenses.getAt(i);
        touch(t.walletID).exp += t.amount;
    }

    AppendLine("--- WALLET-BASED SUMMARY ---");
    AppendLine("");
    AppendLine("Wallet Name             | Income         | Expense        | Balance");
    AppendLine("-------------------------------------------------------------------");
    
    for (int i = 0; i < aggCount; ++i) {
        std::string name = wallets.getWalletName(agg[i].id);
        std::ostringstream row;
        row << std::left << std::setw(25) << name.substr(0, 25)
            << " | " << std::right << std::setw(12) << FormatCurrency((long long)agg[i].inc)
            << " | " << std::right << std::setw(12) << FormatCurrency((long long)agg[i].exp)
            << " | " << std::right << std::setw(12) << FormatCurrency((long long)(agg[i].inc - agg[i].exp));
        AppendLine(row.str());
    }
    delete[] agg;
}

void StatisticWindow::ShowAnnualOverview() {
    if (!dataManager_) return;
    ClearReport();
    auto& incomes = dataManager_->incomes_;
    auto& expenses = dataManager_->expenses_;

    double totalInc = 0.0, totalExp = 0.0;
    int* years = new int[100];
    int yearCount = 0;
    auto addYear = [&](int y){ 
        for (int i = 0; i < yearCount; ++i) if (years[i] == y) return;
        if (yearCount < 100) years[yearCount++] = y; 
    };

    for (int i = 0; i < incomes.getCount(); ++i) { auto t = incomes.getAt(i); totalInc += t.amount; addYear(t.day.year); }
    for (int i = 0; i < expenses.getCount(); ++i) { auto t = expenses.getAt(i); totalExp += t.amount; addYear(t.day.year); }

    std::ostringstream oss;
    AppendLine("--- ANNUAL OVERVIEW ---");
    AppendLine("");
    if (yearCount > 0) {
        oss << "Years included: ";
        for (int i = 0; i < yearCount; ++i) { oss << years[i] << ((i+1<yearCount)?", ":""); }
        AppendLine(oss.str());
        oss.str(""); oss.clear();
    }
    AppendLine("");
    oss << "Total Income:   " << FormatCurrency((long long)totalInc); AppendLine(oss.str()); oss.str(""); oss.clear();
    oss << "Total Expense:  " << FormatCurrency((long long)totalExp); AppendLine(oss.str()); oss.str(""); oss.clear();
    oss << "Net Balance:    " << FormatCurrency((long long)(totalInc - totalExp)); AppendLine(oss.str());
    delete[] years;
}

void StatisticWindow::ShowIncomeBreakdown() {
    if (!dataManager_) return;
    ClearReport();
    auto& incomes = dataManager_->incomes_;
    auto& sources = dataManager_->sources_;

    struct Rec { int id; double amt; };
    Rec* recs = new Rec[100];
    int recCount = 0;
    auto touch = [&](int id) -> Rec& { 
        for (int i = 0; i < recCount; ++i) if (recs[i].id==id) return recs[i]; 
        if (recCount < 100) { recs[recCount].id = id; recs[recCount].amt = 0; return recs[recCount++]; }
        return recs[0]; 
    };

    double total = 0.0;
    for (int i = 0; i < incomes.getCount(); ++i) { auto t = incomes.getAt(i); total += t.amount; touch(t.sourceID).amt += t.amount; }

    AppendLine("--- INCOME BREAKDOWN (by source) ---");
    AppendLine("");
    if (total == 0.0) { AppendLine("(No income data)"); delete[] recs; return; }
    
    AppendLine("Source Name             | Amount         | Percent");
    AppendLine("----------------------------------------------------------");
    for (int i = 0; i < recCount; ++i) {
        std::string name = sources.getSourceName(recs[i].id);
        double pct = (recs[i].amt / total) * 100.0;
        std::ostringstream row;
        row << std::left << std::setw(25) << name.substr(0, 25)
            << " | " << std::right << std::setw(12) << FormatCurrency((long long)recs[i].amt)
            << " | " << std::fixed << std::setprecision(1) << std::setw(6) << pct << "%";
        AppendLine(row.str());
    }
    AppendLine("");
    std::ostringstream totalRow; totalRow << "TOTAL: " << FormatCurrency((long long)total); AppendLine(totalRow.str());
    delete[] recs;
}

void StatisticWindow::ShowExpenseBreakdown() {
    if (!dataManager_) return;
    ClearReport();
    auto& expenses = dataManager_->expenses_;
    auto& categories = dataManager_->categories_;

    struct Rec { int id; double amt; };
    Rec* recs = new Rec[100];
    int recCount = 0;
    auto touch = [&](int id) -> Rec& { 
        for (int i = 0; i < recCount; ++i) if (recs[i].id==id) return recs[i]; 
        if (recCount < 100) { recs[recCount].id = id; recs[recCount].amt = 0; return recs[recCount++]; }
        return recs[0]; 
    };

    double total = 0.0;
    for (int i = 0; i < expenses.getCount(); ++i) { auto t = expenses.getAt(i); total += t.amount; touch(t.categoryID).amt += t.amount; }

    AppendLine("--- EXPENSE BREAKDOWN (by category) ---");
    AppendLine("");
    if (total == 0.0) { AppendLine("(No expense data)"); delete[] recs; return; }
    
    AppendLine("Category Name           | Amount         | Percent");
    AppendLine("----------------------------------------------------------");
    for (int i = 0; i < recCount; ++i) {
        std::string name = categories.getCategoryName(recs[i].id);
        double pct = (recs[i].amt / total) * 100.0;
        std::ostringstream row;
        row << std::left << std::setw(25) << name.substr(0, 25)
            << " | " << std::right << std::setw(12) << FormatCurrency((long long)recs[i].amt)
            << " | " << std::fixed << std::setprecision(1) << std::setw(6) << pct << "%";
        AppendLine(row.str());
    }
    AppendLine("");
    std::ostringstream totalRow; totalRow << "TOTAL: " << FormatCurrency((long long)total); AppendLine(totalRow.str());
    delete[] recs;
}

void StatisticWindow::ShowWalletBalance(int walletID) {
    if (!dataManager_) return;
    ClearReport();

    auto& incomes = dataManager_->incomes_;
    auto& expenses = dataManager_->expenses_;
    auto& wallets = dataManager_->wallets_;

    double totalIncome = 0.0;
    double totalExpense = 0.0;

    // Sum income for this wallet
    for (int i = 0; i < incomes.getCount(); ++i) {
        if (incomes.getAt(i).walletID == walletID) {
            totalIncome += incomes.getAt(i).amount;
        }
    }

    // Sum expense for this wallet
    for (int i = 0; i < expenses.getCount(); ++i) {
        if (expenses.getAt(i).walletID == walletID) {
            totalExpense += expenses.getAt(i).amount;
        }
    }

    double balance = totalIncome - totalExpense;
    std::string walletName = wallets.getWalletName(walletID);

    AppendLine("--- WALLET BALANCE ---");
    AppendLine("");
    AppendLine("Wallet: " + walletName);
    AppendLine("");
    std::ostringstream oss;
    oss << "Total Income:   " << FormatCurrency((long long)totalIncome); AppendLine(oss.str()); oss.str(""); oss.clear();
    oss << "Total Expense:  " << FormatCurrency((long long)totalExpense); AppendLine(oss.str()); oss.str(""); oss.clear();
    AppendLine("");
    oss << "Current Balance: " << FormatCurrency((long long)balance); AppendLine(oss.str());
}
