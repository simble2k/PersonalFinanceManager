#pragma once

#include <string>
#include <sstream>
#include <functional>
#include <iomanip>

#include "raylib.h"
#include "DataManager.h"
#include "ScrollArea.h"
#include "button.h"
#include "LayoutHelper.h"
#include "Dialog.h"
#include "Dropdown.h"
#include "date.h"
#include "statistic.h"

// window settings (consistent with other windows)
static const int STATS_SCREEN_W = 1280;
static const int STATS_SCREEN_H = 800;

class StatisticWindow {
public:
    StatisticWindow();
    ~StatisticWindow();

    void BindDataManager(DataManager* dm) { dataManager_ = dm; }
    void SetOnBack(std::function<void()> cb) { onBackRequested_ = std::move(cb); }

    void Init();
    void Update();
    void Draw(DataManager& dm);

private:
    DataManager* dataManager_ = nullptr;
    bool initialized_ = false;

    // UI controls
    Button backBtn_;
    Button timeBasedBtn_;
    Button walletBasedBtn_;
    Button annualOverviewBtn_;
    Button incomeBreakdownBtn_;
    Button expenseBreakdownBtn_;
    Button walletBalanceBtn_;

    // Dialogs for input
    Dialog dateRangeDialog_;
    Dialog walletSelectDialog_;
    Dialog yearSelectDialog_;
    bool showDateRangeDialog_ = false;
    bool showWalletSelectDialog_ = false;
    bool showYearSelectDialog_ = false;

    // Dropdowns for selection
    Dropdown* walletDropdown_ = nullptr;
    Dropdown* yearDropdown_ = nullptr;

    // Results display
    ScrollArea resultsArea_;
    std::string* reportLines_ = nullptr;
    int reportLineCount_ = 0;
    int reportLineCapacity_ = 0;
    std::string reportTitle_;

    // Styled rendering state for time-based summary
    bool hasTimeBasedData_ = false;
    date tbFrom_{};
    date tbTo_{};
    TimeReport tbReport_{};

    // Styled rendering state for annual overview
    bool hasAnnualOverviewData_ = false;
    TimeReport aoReport_{};
    std::string aoPeriod_{}; // e.g., "2023, 2024"

    // Styled rendering state for wallet balance
    bool hasWalletBalanceData_ = false;
    double wbIncome_ = 0.0;
    double wbExpense_ = 0.0;
    double wbBalance_ = 0.0;
    std::string wbWalletName_{};

    // Styled rendering state for wallet-based summary table
    bool hasWalletBasedStyledData_ = false;
    struct WalletRow { int id; std::string name; double income; double expense; double balance; };
    WalletRow* walletRows_ = nullptr;
    int walletRowCount_ = 0;

    // Styled rendering state for breakdown tables (income/expense)
    bool hasIncomeBreakdownStyledData_ = false;
    bool hasExpenseBreakdownStyledData_ = false;
    struct BreakdownRow { std::string name; double amount; double percentage; };
    BreakdownRow* breakdownRows_ = nullptr;
    int breakdownRowCount_ = 0;
    double breakdownTotal_ = 0.0;

    std::function<void()> onBackRequested_;

    // Error handling
    int errorID_ = -1;

    // helpers to render text
    void ClearReport();
    void AppendLine(const std::string& s);

    // report generators
    void ShowTimeBasedStats(date fromDate, date toDate);
    void ShowWalletBasedStats();
    void ShowAnnualOverview(int* selectedYears, int yearCount);
    void ShowIncomeBreakdown();
    void ShowExpenseBreakdown();
    void ShowWalletBalance(int walletID);

    // Dialog handlers
    void OpenDateRangeDialog();
    void SubmitDateRange();
    void OpenWalletSelectDialog();
    void SubmitWalletSelect();
    void OpenYearSelectDialog();
    void SubmitYearSelect();
    void CloseDialogs();
};
