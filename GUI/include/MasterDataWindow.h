#pragma once

#include <string>
#include <functional>
#include <memory>
#include "raylib.h"
#include "DataManager.h"
#include "ScrollArea.h"
#include "button.h"
#include "LayoutHelper.h"
#include "Helper.h"
#include "Dialog.h"

class MasterDataWindow {
public:
    MasterDataWindow();
    ~MasterDataWindow();

    void BindDataManager(DataManager* dataManager) { dataManager_ = dataManager; }
    void SetOnBack(std::function<void()> cb) { onBackRequested_ = std::move(cb); }

    void Init();
    void Update();
    void Draw(DataManager& dataManager);

private:
    // Selection focus enum
    enum class Focus { None, Wallet, Source, Category };
    enum class FormMode { Add, Edit };

    // UI components
    ScrollArea walletArea_;
    ScrollArea sourceArea_;
    ScrollArea categoryArea_;
    ScrollArea txnArea_;

    Button backBtn_;
    Button addBtn_;
    Button editBtn_;
    Button deleteBtn_;

    Dialog formDialog_;
    Dialog confirmDialog_;
    bool showFormDialog_ = false;
    bool showConfirmDialog_ = false;
    FormMode formMode_ = FormMode::Add;
    Focus formTarget_ = Focus::Wallet;
    int editOriginalId_ = -1;

    Rectangle walletCol_ {0,0,0,0};
    Rectangle sourceCol_ {0,0,0,0};
    Rectangle categoryCol_ {0,0,0,0};
    Rectangle txnCol_ {0,0,0,0};

    float walletContentH_ = 0.0f;
    float sourceContentH_ = 0.0f;
    float categoryContentH_ = 0.0f;
    float txnContentH_ = 0.0f;

    DataManager* dataManager_ = nullptr;
    std::function<void()> onBackRequested_;

    bool initialized_ = false;

    // Selection state
    int selectedWalletId_ = -1;
    int selectedSourceId_ = -1;
    int selectedCategoryId_ = -1;
    Focus focus_ = Focus::None;

    std::string status_;
    Color statusColor_ = DARKGRAY;

    // Layout helpers
    void DrawColumns(DataManager& dataManager);
    void DrawWallets();
    void DrawSourcesAndCategories();
    void DrawTransactions();
    void ClearSelectionsBelowWallet();

    void SetStatus(const std::string& msg, Color color);

    // Mutations
    int NextId(NameTable& table) const;

    void OpenFormDialog(FormMode mode, Focus target);
    void SubmitForm();
    void OpenDeleteConfirm();
    void ConfirmDelete();
    void CloseDialogs();
};
