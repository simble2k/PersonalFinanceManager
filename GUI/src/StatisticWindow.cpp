#include "StatisticWindow.h"

// Layout constants
static const int PADDING = 24;
static const int TITLE_H = 64;
static const int BTN_W = 180;
static const int BTN_H = 44;
static const int BTN_SPACING = 12;
static const int TABLE_Y = 160;
static const int TABLE_H = 560;
static const int LINE_H = 28;

StatisticWindow::StatisticWindow() {}
StatisticWindow::~StatisticWindow() { 
    if (reportLines_) {
        delete[] reportLines_;
        reportLines_ = nullptr;
    }
}

void StatisticWindow::Init() {
    if (initialized_) return;

    // Allocate dynamic report lines array
    reportLineCapacity_ = 256;
    reportLines_ = new std::string[reportLineCapacity_];
    reportLineCount_ = 0;

    // Back button (top-left)
    backBtn_ = Button((float)PADDING, 20.0f, 140.0f, BTN_H, "< Dashboard", LIGHTGRAY, GRAY, BLACK,
                      [this]() { if (onBackRequested_) onBackRequested_(); });

    // Create separate grids for each row to allow larger horizontal spacing
    const float GRID_BTN_W = 250.0f;
    const float GRID_BTN_H = 50.0f;
    const float GRID_SPACING_H = 200.0f;  // Larger horizontal spacing
    const float GRID_SPACING_V = 12.0f;  // Vertical spacing between rows
    const float GRID_START_X = (float)PADDING;
    const float GRID_START_Y1 = 20.0f;
    const float GRID_START_Y2 = GRID_START_Y1 + GRID_BTN_H + GRID_SPACING_V;
    
    // Row 1: Time-based, Wallet-based
    Rectangle* row1Rects = LayoutHelper::CreateGrid(GRID_START_X + GRID_SPACING_H + GRID_BTN_W, GRID_START_Y1, 1, 2, GRID_BTN_W, GRID_BTN_H, GRID_SPACING_H);
    
    timeBasedBtn_ = Button(row1Rects[0].x, row1Rects[0].y, row1Rects[0].width, row1Rects[0].height, "Time-based", GREEN, DARKGREEN, WHITE,
                           [this]() { ShowTimeBasedStats(); });
    walletBasedBtn_ = Button(row1Rects[1].x, row1Rects[1].y, row1Rects[1].width, row1Rects[1].height, "Wallet-based", GREEN, DARKGREEN, WHITE,
                             [this]() { ShowWalletBasedStats(); });
    
    delete[] row1Rects;
    
    // Row 2: Annual Overview, Income Breakdown, Expense Breakdown
    Rectangle* row2Rects = LayoutHelper::CreateGrid(GRID_START_X, GRID_START_Y2, 1, 3, GRID_BTN_W, GRID_BTN_H, GRID_SPACING_H);
    
    annualOverviewBtn_ = Button(row2Rects[0].x, row2Rects[0].y, row2Rects[0].width, row2Rects[0].height, "Annual Overview", SKYBLUE, BLUE, WHITE,
                                [this]() { ShowAnnualOverview(); });
    incomeBreakdownBtn_ = Button(row2Rects[1].x, row2Rects[1].y, row2Rects[1].width, row2Rects[1].height, "Income Breakdown", ORANGE, DARKGRAY, BLACK,
                                 [this]() { ShowIncomeBreakdown(); });
    expenseBreakdownBtn_ = Button(row2Rects[2].x, row2Rects[2].y, row2Rects[2].width, row2Rects[2].height, "Expense Breakdown", PURPLE, DARKPURPLE, WHITE,
                                  [this]() { ShowExpenseBreakdown(); });
    
    delete[] row2Rects;

    // Results area
    resultsArea_.Init((float)PADDING, 200.0f, (float)(STATS_SCREEN_W - PADDING * 2), 540.0f);

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

    // Update scroll area content height
    float contentH = (float)((reportLineCount_ > 0 ? reportLineCount_ : 1) * LINE_H);
    resultsArea_.Update(contentH);
}

void StatisticWindow::Draw(DataManager& dm) {
    // Draw buttons
    backBtn_.Draw();
    timeBasedBtn_.Draw();
    walletBasedBtn_.Draw();
    annualOverviewBtn_.Draw();
    incomeBreakdownBtn_.Draw();
    expenseBreakdownBtn_.Draw();

    // Title (below buttons)
    LayoutHelper::CenterText("REPORTS & STATISTICS", 32, DARKBLUE, STATS_SCREEN_W / 2.0f, 155);

    // Draw results area border
    DrawRectangleLinesEx(Rectangle{ (float)PADDING, 200.0f, (float)(STATS_SCREEN_W - PADDING * 2), 540.0f }, 2.0f, LIGHTGRAY);

    // Clip and draw report lines
    resultsArea_.Begin();
    float y = 200.0f - resultsArea_.GetOffset() + 8.0f;
    for (int i = 0; i < reportLineCount_; ++i) {
        DrawText(reportLines_[i].c_str(), PADDING + 12, y, 20, BLACK);
        y += LINE_H;
    }
    resultsArea_.End();
}

void StatisticWindow::ClearReport() { reportLineCount_ = 0; }
void StatisticWindow::Append(const std::string& s) { 
    if (reportLineCount_ < reportLineCapacity_) reportLines_[reportLineCount_++] = s; 
}
void StatisticWindow::AppendLine(const std::string& s) { 
    if (reportLineCount_ < reportLineCapacity_) reportLines_[reportLineCount_++] = s; 
}

// --- Report Generators (GUI-friendly, non-interactive) ---

static bool lessOrEq(date a, date b) { return compareDate(a, b) <= 0; }
static bool greaterOrEq(date a, date b) { return compareDate(a, b) >= 0; }

static date minDate(date a, date b) { return (compareDate(a, b) <= 0) ? a : b; }
static date maxDate(date a, date b) { return (compareDate(a, b) >= 0) ? a : b; }

void StatisticWindow::ShowTimeBasedStats() {
    if (!dataManagerRef_) return;
    ClearReport();

    auto& incomes = dataManagerRef_->incomes_;
    auto& expenses = dataManagerRef_->expenses_;

    if (incomes.getCount() == 0 && expenses.getCount() == 0) {
        AppendLine("No transactions yet.");
        return;
    }

    // Determine full date range
    date from{31,12,9999};
    date to{1,1,1};
    for (int i = 0; i < incomes.getCount(); ++i) {
        IncomeTransaction t = incomes.getAt(i);
        from = minDate(from, t.day);
        to = maxDate(to, t.day);
    }
    for (int i = 0; i < expenses.getCount(); ++i) {
        ExpenseTransaction t = expenses.getAt(i);
        from = minDate(from, t.day);
        to = maxDate(to, t.day);
    }

    double totalInc = 0.0, totalExp = 0.0;
    for (int i = 0; i < incomes.getCount(); ++i) {
        IncomeTransaction t = incomes.getAt(i);
        if (greaterOrEq(t.day, from) && lessOrEq(t.day, to)) totalInc += t.amount;
    }
    for (int i = 0; i < expenses.getCount(); ++i) {
        ExpenseTransaction t = expenses.getAt(i);
        if (greaterOrEq(t.day, from) && lessOrEq(t.day, to)) totalExp += t.amount;
    }

    char fromBuf[16], toBuf[16];
    snprintf(fromBuf, sizeof(fromBuf), "%02d/%02d/%04d", from.day, from.month, from.year);
    snprintf(toBuf, sizeof(toBuf), "%02d/%02d/%04d", to.day, to.month, to.year);

    AppendLine("--- TIME-BASED SUMMARY ---");
    AppendLine(std::string("From: ") + fromBuf + "  To: " + toBuf);
    std::ostringstream oss;
    oss << "Total Income: " << (long long)totalInc; AppendLine(oss.str()); oss.str(""); oss.clear();
    oss << "Total Expense: " << (long long)totalExp; AppendLine(oss.str()); oss.str(""); oss.clear();
    oss << "Net: " << (long long)(totalInc - totalExp); AppendLine(oss.str());
}

void StatisticWindow::ShowWalletBasedStats() {
    if (!dataManagerRef_) return;
    ClearReport();

    auto& incomes = dataManagerRef_->incomes_;
    auto& expenses = dataManagerRef_->expenses_;
    auto& wallets = dataManagerRef_->wallets_;

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

    AppendLine("--- BY WALLET ---");
    AppendLine("Wallet                      | Income         | Expense");
    AppendLine("------------------------------------------------------------");
    for (int i = 0; i < aggCount; ++i) {
        std::string name = wallets.getWalletName(agg[i].id);
        std::ostringstream row;
        row << std::left << std::setw(26) << name.substr(0,26)
            << " | " << std::right << std::setw(12) << (long long)agg[i].inc
            << " | " << std::right << std::setw(12) << (long long)agg[i].exp;
        AppendLine(row.str());
    }
    delete[] agg;
}

void StatisticWindow::ShowAnnualOverview() {
    if (!dataManagerRef_) return;
    ClearReport();
    auto& incomes = dataManagerRef_->incomes_;
    auto& expenses = dataManagerRef_->expenses_;

    double totalInc = 0.0, totalExp = 0.0;
    int* years = new int[100];
    int yearCount = 0;
    auto addYear = [&](int y){ 
        for (int i = 0; i < yearCount; ++i) if (years[i] == y) return;  // already exists
        if (yearCount < 100) years[yearCount++] = y; 
    };

    for (int i = 0; i < incomes.getCount(); ++i) { auto t = incomes.getAt(i); totalInc += t.amount; addYear(t.day.year); }
    for (int i = 0; i < expenses.getCount(); ++i) { auto t = expenses.getAt(i); totalExp += t.amount; addYear(t.day.year); }

    std::ostringstream oss;
    AppendLine("--- ANNUAL OVERVIEW ---");
    if (yearCount > 0) {
        oss << "Years included: ";
        for (int i = 0; i < yearCount; ++i) { oss << years[i] << ((i+1<yearCount)?", ":""); }
        AppendLine(oss.str());
        oss.str(""); oss.clear();
    }
    oss << "Total Income: " << (long long)totalInc; AppendLine(oss.str()); oss.str(""); oss.clear();
    oss << "Total Expense: " << (long long)totalExp; AppendLine(oss.str()); oss.str(""); oss.clear();
    oss << "Net Balance: " << (long long)(totalInc - totalExp); AppendLine(oss.str());
    delete[] years;
}

void StatisticWindow::ShowIncomeBreakdown() {
    if (!dataManagerRef_) return;
    ClearReport();
    auto& incomes = dataManagerRef_->incomes_;
    auto& sources = dataManagerRef_->sources_;

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
    if (total == 0.0) { AppendLine("(No income data)"); delete[] recs; return; }
    AppendLine("Source                     | Amount         | Percent");
    AppendLine("------------------------------------------------------------");
    for (int i = 0; i < recCount; ++i) {
        std::string name = sources.getSourceName(recs[i].id);
        double pct = (recs[i].amt / total) * 100.0;
        std::ostringstream row;
        row << std::left << std::setw(26) << name.substr(0,26)
            << " | " << std::right << std::setw(12) << (long long)recs[i].amt
            << " | " << std::fixed << std::setprecision(1) << std::setw(6) << pct << "%";
        AppendLine(row.str());
    }
    std::ostringstream totalRow; totalRow << "TOTAL: " << (long long)total; AppendLine(totalRow.str());
    delete[] recs;
}

void StatisticWindow::ShowExpenseBreakdown() {
    if (!dataManagerRef_) return;
    ClearReport();
    auto& expenses = dataManagerRef_->expenses_;
    auto& categories = dataManagerRef_->categories_;

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
    if (total == 0.0) { AppendLine("(No expense data)"); delete[] recs; return; }
    AppendLine("Category                    | Amount         | Percent");
    AppendLine("------------------------------------------------------------");
    for (int i = 0; i < recCount; ++i) {
        std::string name = categories.getCategoryName(recs[i].id);
        double pct = (recs[i].amt / total) * 100.0;
        std::ostringstream row;
        row << std::left << std::setw(26) << name.substr(0,26)
            << " | " << std::right << std::setw(12) << (long long)recs[i].amt
            << " | " << std::fixed << std::setprecision(1) << std::setw(6) << pct << "%";
        AppendLine(row.str());
    }
    std::ostringstream totalRow; totalRow << "TOTAL: " << (long long)total; AppendLine(totalRow.str());
    delete[] recs;
}
