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
    if (yearDropdown_) {
        delete yearDropdown_;
        yearDropdown_ = nullptr;
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
                                [this]() { OpenYearSelectDialog(); });
    
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

    // Initialize date range dialog (extra spacing and room for labels)
    dateRangeDialog_ = Dialog(STATS_SCREEN_W / 2.0f - 200.0f, STATS_SCREEN_H / 2.0f - 165.0f, 400.0f, 330.0f, "Enter Date Range");
    // Inputs moved down to create more spacing between them
    dateRangeDialog_.AddTextInput(20, 70, 360, 40, "Start Date (DD/MM/YYYY)");
    dateRangeDialog_.AddTextInput(20, 150, 360, 40, "End Date (DD/MM/YYYY)");
    // Buttons moved down to keep space between inputs and buttons
    dateRangeDialog_.AddButton(60, 230, 120, 40, "OK", BLUE, DARKBLUE, WHITE,
                               [this]() { SubmitDateRange(); });
    dateRangeDialog_.AddButton(220, 230, 120, 40, "Cancel", RED, MAROON, WHITE,
                               [this]() { CloseDialogs(); });

    // Initialize wallet select dialog
    walletSelectDialog_ = Dialog(STATS_SCREEN_W / 2.0f - 200.0f, STATS_SCREEN_H / 2.0f - 100.0f, 400.0f, 250.0f, "Select Wallet");
    walletSelectDialog_.AddButton(60, 140, 120, 40, "OK", BLUE, DARKBLUE, WHITE,
                                  [this]() { SubmitWalletSelect(); });
    walletSelectDialog_.AddButton(220, 140, 120, 40, "Cancel", RED, MAROON, WHITE,
                                  [this]() { CloseDialogs(); });

    // Initialize year select dialog
    yearSelectDialog_ = Dialog(STATS_SCREEN_W / 2.0f - 200.0f, STATS_SCREEN_H / 2.0f - 100.0f, 400.0f, 250.0f, "Select Year");
    yearSelectDialog_.AddButton(60, 140, 120, 40, "OK", BLUE, DARKBLUE, WHITE,
                                [this]() { SubmitYearSelect(); });
    yearSelectDialog_.AddButton(220, 140, 120, 40, "Cancel", RED, MAROON, WHITE,
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
    if (showYearSelectDialog_) {
        yearSelectDialog_.Update();
        if (yearDropdown_) {
            yearDropdown_->Update();
        }
    }

    // Update scroll area content height
    float contentH = (float)((reportLineCount_ > 0 ? reportLineCount_ : 1) * LINE_H);
    resultsArea_.Update(contentH);
}

void StatisticWindow::Draw(DataManager& dm) {
    // Draw title
    LayoutHelper::CenterText("REPORTS & STATISTICS", 36, DARKBLUE, STATS_SCREEN_W / 2.0f, 20.0f);

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
    DrawText("--- FEATURES ---", RIGHT_COLUMN_START_X + 20, PADDING + 20, 27, DARKBLUE);

    // Draw results area border and content
    DrawRectangleLinesEx(Rectangle{ (float)PADDING, PADDING + 80.0f, 
                                    (float)(LEFT_COLUMN_WIDTH - 20), 
                                    (float)(STATS_SCREEN_H - PADDING * 2 - 80) }, 2.0f, LIGHTGRAY);

    // Draw report title centered at top of results area
    if (!reportTitle_.empty()) {
        int titleWidth = MeasureText(reportTitle_.c_str(), 20);
        float titleX = PADDING + (LEFT_COLUMN_WIDTH - 20 - titleWidth) / 2.0f;
        DrawText(reportTitle_.c_str(), titleX, PADDING + 90, 20, DARKBLUE);
        
        // Draw separator line below title
        DrawLine(PADDING + 10, PADDING + 115, PADDING + LEFT_COLUMN_WIDTH - 30, PADDING + 115, LIGHTGRAY);
    }

    // Clip and draw content
    resultsArea_.Begin();
    float contentTop = PADDING + 80.0f - resultsArea_.GetOffset();
    if (hasTimeBasedData_) {
        // Styled time-based summary with right-aligned currency
        char fromBuf[32], toBuf[32];
        snprintf(fromBuf, sizeof(fromBuf), "%02d/%02d/%04d", tbFrom_.day, tbFrom_.month, tbFrom_.year);
        snprintf(toBuf, sizeof(toBuf), "%02d/%02d/%04d", tbTo_.day, tbTo_.month, tbTo_.year);

        float xLabel = PADDING + 12.0f;
        float rightEdge = PADDING + (LEFT_COLUMN_WIDTH - 20);
        const int padRight = 100;
        float y = contentTop + (reportTitle_.empty() ? 10.0f : 50.0f);

        // Period line just below the title
        std::string period = std::string("Period: ") + fromBuf + " to " + toBuf;
        DrawText(period.c_str(), (int)xLabel, (int)y, 22, DARKGRAY);
        y += 50.0f;

        auto drawAmount = [&](const char* label, const std::string& amountStr, int labelSize, Color labelColor, int valueSize, Color valueColor) {
            DrawText(label, (int)xLabel, (int)y, labelSize, labelColor);
            int tw = MeasureText(amountStr.c_str(), valueSize);
            int vx = (int)(rightEdge - padRight - tw);
            DrawText(amountStr.c_str(), vx, (int)y, valueSize, valueColor);
            y += (float)(valueSize + 20);
        };

        drawAmount("Total Income:", FormatCurrency((long long)tbReport_.totalIncome), 24, BLACK, 30, GREEN);
        drawAmount("Total Expense:", FormatCurrency((long long)tbReport_.totalExpense), 24, BLACK, 30, RED);
        drawAmount("Net Balance:",  FormatCurrency((long long)tbReport_.netBalance),  28, BLACK, 35, DARKBLUE);
    } else if (hasAnnualOverviewData_) {
        // Styled annual overview with right-aligned currency
        float xLabel = PADDING + 12.0f;
        float rightEdge = PADDING + (LEFT_COLUMN_WIDTH - 20);
        const int padRight = 100;

        float y = contentTop + (reportTitle_.empty() ? 10.0f : 50.0f);

        // Period under title
        std::string periodText = std::string("Years: ") + aoPeriod_;
        DrawText(periodText.c_str(), (int)xLabel, (int)y, 22, DARKGRAY);
        y += 50.0f;

        auto drawAmount = [&](const char* label, const std::string& amountStr, int labelSize, Color labelColor, int valueSize, Color valueColor) {
            DrawText(label, (int)xLabel, (int)y, labelSize, labelColor);
            int tw = MeasureText(amountStr.c_str(), valueSize);
            int vx = (int)(rightEdge - padRight - tw);
            DrawText(amountStr.c_str(), vx, (int)y, valueSize, valueColor);
            y += (float)(valueSize + 20);
        };

        drawAmount("Total Income:", FormatCurrency((long long)aoReport_.totalIncome), 24, BLACK, 30, GREEN);
        drawAmount("Total Expense:", FormatCurrency((long long)aoReport_.totalExpense), 24, BLACK, 30, RED);
        drawAmount("Net Balance:",  FormatCurrency((long long)aoReport_.netBalance),  28, BLACK, 35, DARKBLUE);
    } else if (hasWalletBasedStyledData_) {
        // Styled wallet-based table with 4 columns: Wallet Name | Income | Expense | Balance
        float xLabel = PADDING + 12.0f;
        float rightEdge = PADDING + (LEFT_COLUMN_WIDTH - 20);
        float availableWidth = rightEdge - xLabel;
        
        // Column widths spread evenly
        float colWidth = availableWidth / 4.0f;
        float col1X = xLabel;
        float col2X = col1X + colWidth;
        float col3X = col2X + colWidth +20.0f;
        float col4X = col3X + colWidth;
        
        float y = contentTop + (reportTitle_.empty() ? 10.0f : 50.0f);

        // Draw header row
        DrawText("Wallet Name", (int)col1X, (int)y, 22, DARKGRAY);
        DrawText("Income", (int)(col2X + colWidth / 2.0f - MeasureText("Income", 22) / 2.0f), (int)y, 22, DARKGRAY);
        DrawText("Expense", (int)(col3X + colWidth / 2.0f - MeasureText("Expense", 22) / 2.0f), (int)y, 22, DARKGRAY);
        DrawText("Balance", (int)(rightEdge - 20 - MeasureText("Balance", 22)), (int)y, 22, DARKGRAY);
        y += 32.0f;

        // Draw separator line below header
        DrawLine((int)xLabel, (int)y, (int)(rightEdge - 10), (int)y, LIGHTGRAY);
        y += 20.0f;

        // Draw data rows
        for (int i = 0; i < walletRowCount_; ++i) {
            // Wallet Name (left-aligned in first column)
            std::string name = walletRows_[i].name;
            if (name.length() > 20) name = name.substr(0, 20);
            DrawText(name.c_str(), (int)col1X, (int)y, 20, BLACK);

            // Income (centered in second column)
            std::string incStr = FormatCurrency((long long)walletRows_[i].income);
            int incW = MeasureText(incStr.c_str(), 20);
            DrawText(incStr.c_str(), (int)(col2X + colWidth / 2.0f - incW / 2.0f), (int)y, 20, GREEN);

            // Expense (centered in third column)
            std::string expStr = FormatCurrency((long long)walletRows_[i].expense);
            int expW = MeasureText(expStr.c_str(), 20);
            DrawText(expStr.c_str(), (int)(col3X + colWidth / 2.0f - expW / 2.0f), (int)y, 20, RED);

            // Balance (right-aligned with 20px padding from right edge)
            std::string balStr = FormatCurrency((long long)walletRows_[i].balance);
            int balW = MeasureText(balStr.c_str(), 20);
            Color balColor = walletRows_[i].balance >= 0 ? DARKBLUE : RED;
            DrawText(balStr.c_str(), (int)(rightEdge - 20 - balW), (int)y, 20, balColor);

            y += 30.0f;
        }
    } else if (hasIncomeBreakdownStyledData_ || hasExpenseBreakdownStyledData_) {
        // Styled breakdown table with 3 columns: Name (left+20) | Amount (middle, right-aligned) | Percent (right-20, right-aligned)
        float xLabel = PADDING + 12.0f + 20.0f; // left bound + 20px padding
        float rightEdge = PADDING + (LEFT_COLUMN_WIDTH - 20);
        float rightPad = 20.0f;
        float amountAlignPos = rightEdge - 150.0f; // position where amount value aligns right
        
        float y = contentTop + (reportTitle_.empty() ? 10.0f : 50.0f);

        // Draw header row
        DrawText("Source/Category", (int)xLabel, (int)y, 22, DARKGRAY);
        int amountHeaderW = MeasureText("Amount", 22);
        DrawText("Amount", (int)(amountAlignPos - amountHeaderW), (int)y, 22, DARKGRAY);
        DrawText("Percent", (int)(rightEdge - rightPad - MeasureText("Percent", 22)), (int)y, 22, DARKGRAY);
        y += 32.0f;

        // Draw separator line below header
        DrawLine((int)(xLabel - 20.0f), (int)y, (int)(rightEdge - 10), (int)y, LIGHTGRAY);
        y += 20.0f;

        // Draw data rows (inside scroll area)
        for (int i = 0; i < breakdownRowCount_; ++i) {
            std::string name = breakdownRows_[i].name;
            if (name.length() > 25) name = name.substr(0, 25);
            DrawText(name.c_str(), (int)xLabel, (int)y, 20, BLACK);

            // Amount (middle column, right-aligned)
            std::string amtStr = FormatCurrency((long long)breakdownRows_[i].amount);
            int amtW = MeasureText(amtStr.c_str(), 20);
            DrawText(amtStr.c_str(), (int)(amountAlignPos - amtW), (int)y, 20, hasIncomeBreakdownStyledData_ ? GREEN : RED);

            // Percent (right column, right-aligned)
            char percentBuf[16];
            snprintf(percentBuf, sizeof(percentBuf), "%.1f%%", breakdownRows_[i].percentage);
            int pctW = MeasureText(percentBuf, 20);
            DrawText(percentBuf, (int)(rightEdge - rightPad - pctW), (int)y, 20, DARKGRAY);

            y += 28.0f;
        }
        
        // Note: TOTAL will be drawn outside the scroll area (after resultsArea_.End())
    } else if (hasWalletBalanceData_) {
        // Styled wallet balance view with right-aligned currency
        float xLabel = PADDING + 12.0f;
        float rightEdge = PADDING + (LEFT_COLUMN_WIDTH - 20);
        const int padRight = 100;

        float y = contentTop + (reportTitle_.empty() ? 10.0f : 50.0f);

        auto drawAmount = [&](const char* label, const std::string& amountStr, int labelSize, Color labelColor, int valueSize, Color valueColor) {
            DrawText(label, (int)xLabel, (int)y, labelSize, labelColor);
            int tw = MeasureText(amountStr.c_str(), valueSize);
            int vx = (int)(rightEdge - padRight - tw);
            DrawText(amountStr.c_str(), vx, (int)y, valueSize, valueColor);
            y += (float)(valueSize + 20);
        };

        drawAmount("Total Income:",   FormatCurrency((long long)wbIncome_),  24, BLACK, 30, GREEN);
        drawAmount("Total Expense:",  FormatCurrency((long long)wbExpense_), 24, BLACK, 30, RED);
        drawAmount("Current Balance:",FormatCurrency((long long)wbBalance_), 28, BLACK, 35, DARKBLUE);
    } else {
        float y = contentTop + (reportTitle_.empty() ? 8.0f : 130.0f);
        for (int i = 0; i < reportLineCount_; ++i) {
            DrawText(reportLines_[i].c_str(), PADDING + 12, y, 22, BLACK);
            y += LINE_H;
        }
    }
    resultsArea_.End();

    // Draw TOTAL for breakdown views (outside scroll area, so it doesn't scroll)
    if (hasIncomeBreakdownStyledData_ || hasExpenseBreakdownStyledData_) {
        float xLabel = PADDING + 12.0f + 20.0f;
        float resultsBottom = PADDING + 80.0f + (STATS_SCREEN_H - PADDING * 2 - 80);
        std::string totalStr = std::string("TOTAL: ") + FormatCurrency((long long)breakdownTotal_);
        Color totalColor = hasIncomeBreakdownStyledData_ ? GREEN : RED;
        DrawText(totalStr.c_str(), (int)(xLabel - 20.0f), (int)(resultsBottom - 40), 22, totalColor);
    }

    // Draw dialogs on top
    if (showDateRangeDialog_) {
        dateRangeDialog_.Draw();
        // Labels above each input box
        Rectangle r = dateRangeDialog_.GetRect();
        DrawText("Start Date", (int)r.x + 20, (int)r.y + 50, 16, DARKGRAY);
        DrawText("End Date",   (int)r.x + 20, (int)r.y + 130, 16, DARKGRAY);
        // Error indicator positioned between inputs and buttons
        if (errorID_ == 2 || errorID_ == 3) {
            DrawFormErrorTextIndicator(Rectangle{ r.x + 400, r.y + 280, 1, 1 }, errorID_);
        }
    }
    if (showWalletSelectDialog_) {
        walletSelectDialog_.Draw();
        if (walletDropdown_) {
            walletDropdown_->DrawBase();
        }
        // Error indicator just under the dropdown inside the dialog
        if (errorID_ == 4) {
            Rectangle r = walletSelectDialog_.GetRect();
            DrawFormErrorTextIndicator(Rectangle{ r.x + 400, r.y + 200, 1, 1 }, errorID_);
        }
    }
    if (showYearSelectDialog_) {
        yearSelectDialog_.Draw();
        if (yearDropdown_) {
            yearDropdown_->DrawBase();
        }
        // Error indicator just under the dropdown inside the dialog
        if (errorID_ == 5) {
            Rectangle r = yearSelectDialog_.GetRect();
            DrawFormErrorTextIndicator(Rectangle{ r.x + 400, r.y + 200, 1, 1 }, errorID_);
        }
    }

    // Draw dropdown overlay last
    if (showWalletSelectDialog_ && walletDropdown_ && walletDropdown_->IsOpen()) {
        walletDropdown_->DrawListOverlay();
    }
    if (showYearSelectDialog_ && yearDropdown_ && yearDropdown_->IsOpen()) {
        yearDropdown_->DrawListOverlay();
    }
}


void StatisticWindow::ClearReport() { 
    reportLineCount_ = 0; 
    reportTitle_ = "";
    hasTimeBasedData_ = false;
    hasAnnualOverviewData_ = false;
    hasWalletBalanceData_ = false;
    hasWalletBasedStyledData_ = false;
    hasIncomeBreakdownStyledData_ = false;
    hasExpenseBreakdownStyledData_ = false;
    if (walletRows_) { delete[] walletRows_; walletRows_ = nullptr; }
    walletRowCount_ = 0;
    if (breakdownRows_) { delete[] breakdownRows_; breakdownRows_ = nullptr; }
    breakdownRowCount_ = 0;
    breakdownTotal_ = 0.0;
}

void StatisticWindow::AppendLine(const std::string& s) { 
    if (reportLineCount_ < reportLineCapacity_) reportLines_[reportLineCount_++] = s; 
}

void StatisticWindow::OpenDateRangeDialog() {
    dateRangeDialog_.ResetInputs();
    dateRangeDialog_.Open();
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

    // Recreate dropdown with wallet options
    if (walletDropdown_) {
        delete walletDropdown_;
    }
    {
        Rectangle r = walletSelectDialog_.GetRect();
        float dx = r.x + 20.0f;
        float dy = r.y + 60.0f;
        float dw = r.width - 40.0f;
        walletDropdown_ = new Dropdown(dx, dy, dw, 40.0f, "Select Wallet");
    }

    // Add all wallets to dropdown
    int walletCount = 0;
    int* walletIds = dataManager_->wallets_.getAllIDs(walletCount);
    
    for (int i = 0; i < walletCount; i++) {
        int walletId = walletIds[i];
        std::string walletName = dataManager_->wallets_.getWalletName(walletId);
        walletDropdown_->AddOption(walletName, walletId);
    }

    delete[] walletIds;

    walletSelectDialog_.Open();
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
    showYearSelectDialog_ = false;
    dateRangeDialog_.Close();
    walletSelectDialog_.Close();
    yearSelectDialog_.Close();
}

void StatisticWindow::OpenYearSelectDialog() {
    if (!dataManager_) return;

    // Recreate dropdown with year options
    if (yearDropdown_) {
        delete yearDropdown_;
    }
    {
        Rectangle r = yearSelectDialog_.GetRect();
        float dx = r.x + 20.0f;
        float dy = r.y + 60.0f;
        float dw = r.width - 40.0f;
        yearDropdown_ = new Dropdown(dx, dy, dw, 40.0f, "Select Year");
    }

    // Get all unique years from transactions
    int* years = new int[100];
    int yearCount = 0;
    auto addYear = [&](int y){
        for (int i = 0; i < yearCount; ++i) if (years[i] == y) return;
        if (yearCount < 100) years[yearCount++] = y;
    };

    for (int i = 0; i < dataManager_->incomes_.getCount(); ++i) {
        addYear(dataManager_->incomes_.getAt(i).day.year);
    }
    for (int i = 0; i < dataManager_->expenses_.getCount(); ++i) {
        addYear(dataManager_->expenses_.getAt(i).day.year);
    }

    // Add years to dropdown
    for (int i = 0; i < yearCount; ++i) {
        char yearStr[16];
        snprintf(yearStr, sizeof(yearStr), "%d", years[i]);
        yearDropdown_->AddOption(yearStr, years[i]);
    }

    delete[] years;

    yearSelectDialog_.Open();
    showYearSelectDialog_ = true;
    errorID_ = -1;
}

void StatisticWindow::SubmitYearSelect() {
    if (!yearDropdown_) return;

    int selectedYear = yearDropdown_->GetSelectedValue();
    if (selectedYear < 0) {
        errorID_ = 5; // Error ID 5 for no year selected
        return;
    }

    errorID_ = -1;

    // Create array with single selected year
    int* selectedYears = new int[1];
    selectedYears[0] = selectedYear;
    ShowAnnualOverview(selectedYears, 1);
    delete[] selectedYears;

    CloseDialogs();
}
void StatisticWindow::ShowTimeBasedStats(date fromDate, date toDate) {
    if (!dataManager_) return;
    ClearReport();

    // Call the statistic function from statistic.cpp
    TimeReport report = getStatTimeBased(fromDate, toDate, dataManager_->incomes_, dataManager_->expenses_);

    // Store for styled drawing
    reportTitle_ = "TIME-BASED SUMMARY";
    hasTimeBasedData_ = true;
    tbFrom_ = fromDate;
    tbTo_ = toDate;
    tbReport_ = report;
}

void StatisticWindow::ShowWalletBasedStats() {
    if (!dataManager_) return;
    ClearReport();

    hasTimeBasedData_ = false;
    hasWalletBasedStyledData_ = true;
    reportTitle_ = "WALLET-BASED SUMMARY";

    // Aggregate per wallet (dynamic array, max 100 wallets)
    struct WalletAgg { int id; double inc, exp; };
    WalletAgg* agg = new WalletAgg[100];
    int aggCount = 0;

    auto touch = [&](int id) -> WalletAgg& {
        for (int i = 0; i < aggCount; ++i) if (agg[i].id == id) return agg[i];
        if (aggCount < 100) { agg[aggCount].id = id; agg[aggCount].inc = 0; agg[aggCount].exp = 0; return agg[aggCount++]; }
        return agg[0];
    };

    // Aggregate income by wallet
    for (int i = 0; i < dataManager_->incomes_.getCount(); ++i) {
        auto t = dataManager_->incomes_.getAt(i);
        touch(t.walletID).inc += t.amount;
    }

    // Aggregate expense by wallet
    for (int i = 0; i < dataManager_->expenses_.getCount(); ++i) {
        auto t = dataManager_->expenses_.getAt(i);
        touch(t.walletID).exp += t.amount;
    }

    // Store results in styled state
    walletRows_ = new WalletRow[aggCount];
    walletRowCount_ = aggCount;
    for (int i = 0; i < aggCount; ++i) {
        walletRows_[i].id = agg[i].id;
        walletRows_[i].name = dataManager_->wallets_.getWalletName(agg[i].id);
        walletRows_[i].income = agg[i].inc;
        walletRows_[i].expense = agg[i].exp;
        walletRows_[i].balance = agg[i].inc - agg[i].exp;
    }

    delete[] agg;
}

void StatisticWindow::ShowAnnualOverview(int* selectedYears, int yearCount) {
    if (!dataManager_) return;
    ClearReport();

    // Get annual overview using selected years
    TimeReport report = getAnnualOverview(selectedYears, yearCount, dataManager_->incomes_, dataManager_->expenses_);

    // Prepare custom draw state (no AppendLine, no stringstream)
    hasTimeBasedData_ = false;
    hasAnnualOverviewData_ = true;
    reportTitle_ = "ANNUAL OVERVIEW";
    aoReport_ = report;

    // Build period text like "2023, 2024"
    aoPeriod_.clear();
    for (int i = 0; i < yearCount; ++i) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", selectedYears[i]);
        if (!aoPeriod_.empty()) aoPeriod_ += ", ";
        aoPeriod_ += buf;
    }
}

void StatisticWindow::ShowIncomeBreakdown() {
    if (!dataManager_) return;
    ClearReport();

    hasTimeBasedData_ = false;
    hasIncomeBreakdownStyledData_ = true;
    // Get all unique years from income transactions
    int* years = new int[100];
    int yearCount = 0;
    auto addYear = [&](int y){
        for (int i = 0; i < yearCount; ++i) if (years[i] == y) return;
        if (yearCount < 100) years[yearCount++] = y;
    };

    for (int i = 0; i < dataManager_->incomes_.getCount(); ++i) {
        addYear(dataManager_->incomes_.getAt(i).day.year);
    }

    // Get income breakdown using all years
    BreakdownReport report = incomeAnnualBreakdown(years, yearCount, dataManager_->incomes_, dataManager_->sources_);

    reportTitle_ = "INCOME BREAKDOWN (by source)";
    
    if (report.count == 0) {
        AppendLine("(No income data)");
        delete[] years;
        return;
    }

    // Store results in styled state
    breakdownRows_ = new BreakdownRow[report.count];
    breakdownRowCount_ = report.count;
    breakdownTotal_ = report.totalAmount;
    
    for (int i = 0; i < report.count; ++i) {
        breakdownRows_[i].name = report.items[i].name;
        breakdownRows_[i].amount = report.items[i].amount;
        breakdownRows_[i].percentage = report.items[i].percentage;
    }

    delete[] report.items;
    delete[] years;
}

void StatisticWindow::ShowExpenseBreakdown() {
    if (!dataManager_) return;
    ClearReport();

    hasTimeBasedData_ = false;
    hasExpenseBreakdownStyledData_ = true;
    // Get all unique years from expense transactions
    int* years = new int[100];
    int yearCount = 0;
    auto addYear = [&](int y){
        for (int i = 0; i < yearCount; ++i) if (years[i] == y) return;
        if (yearCount < 100) years[yearCount++] = y;
    };

    for (int i = 0; i < dataManager_->expenses_.getCount(); ++i) {
        addYear(dataManager_->expenses_.getAt(i).day.year);
    }

    // Get expense breakdown using all years
    BreakdownReport report = expenseAnnualBreakdown(years, yearCount, dataManager_->expenses_, dataManager_->categories_);

    reportTitle_ = "EXPENSE BREAKDOWN (by category)";
    
    if (report.count == 0) {
        AppendLine("(No expense data)");
        delete[] years;
        return;
    }

    // Store results in styled state
    breakdownRows_ = new BreakdownRow[report.count];
    breakdownRowCount_ = report.count;
    breakdownTotal_ = report.totalAmount;
    
    for (int i = 0; i < report.count; ++i) {
        breakdownRows_[i].name = report.items[i].name;
        breakdownRows_[i].amount = report.items[i].amount;
        breakdownRows_[i].percentage = report.items[i].percentage;
    }

    delete[] report.items;
    delete[] years;
}

void StatisticWindow::ShowWalletBalance(int walletID) {
    if (!dataManager_) return;
    ClearReport();

    hasTimeBasedData_ = false;
    hasAnnualOverviewData_ = false;
    hasWalletBalanceData_ = true;
    // Call the statistic function from statistic.cpp
    double balance = getWalletBalance(walletID, dataManager_->incomes_, dataManager_->expenses_);

    // Calculate income and expense totals for this wallet
    double totalIncome = 0.0;
    double totalExpense = 0.0;

    for (int i = 0; i < dataManager_->incomes_.getCount(); ++i) {
        if (dataManager_->incomes_.getAt(i).walletID == walletID) {
            totalIncome += dataManager_->incomes_.getAt(i).amount;
        }
    }

    for (int i = 0; i < dataManager_->expenses_.getCount(); ++i) {
        if (dataManager_->expenses_.getAt(i).walletID == walletID) {
            totalExpense += dataManager_->expenses_.getAt(i).amount;
        }
    }

    std::string walletName = dataManager_->wallets_.getWalletName(walletID);

    reportTitle_ = "WALLET BALANCE - " + walletName;
    wbWalletName_ = walletName;
    wbIncome_ = totalIncome;
    wbExpense_ = totalExpense;
    wbBalance_ = balance;
}
