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
#include "date.h"
#include "statistic.h"
// window settings (consistent with other windows)
static const int STATS_SCREEN_W = 1280;
static const int STATS_SCREEN_H = 800;

class StatisticWindow {
public:
    StatisticWindow();
    ~StatisticWindow();

    void BindDataManager(DataManager* dm) { dataManagerRef_ = dm; }
    void SetOnBack(std::function<void()> cb) { onBackRequested_ = std::move(cb); }

    void Init();
    void Update();
    void Draw(DataManager& dm);

private:
    DataManager* dataManagerRef_ = nullptr;
    bool initialized_ = false;

    // UI controls
    Button backBtn_;
    Button timeBasedBtn_;
    Button walletBasedBtn_;
    Button annualOverviewBtn_;
    Button incomeBreakdownBtn_;
    Button expenseBreakdownBtn_;

    ScrollArea resultsArea_;
    std::string* reportLines_ = nullptr;
    int reportLineCount_ = 0;
    int reportLineCapacity_ = 0;

    std::function<void()> onBackRequested_;

    // helpers to render text
    void ClearReport();
    void Append(const std::string& s);
    void AppendLine(const std::string& s);

    // report generators
    void ShowTimeBasedStats(); // full-range by default
    void ShowWalletBasedStats(); // full-range by default
    void ShowAnnualOverview(); // all years
    void ShowIncomeBreakdown(); // all years
    void ShowExpenseBreakdown(); // all years
};
