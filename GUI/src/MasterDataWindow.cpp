#include "MasterDataWindow.h"
#include "NameTable.h"
#include "Helper.h"
#include <string>

// Layout constants tuned for 1280x800 window
static const int SCREEN_W = 1280;
static const int SCREEN_H = 800;
static const int PADDING = 24;       
static const int TITLE_H = 72;
static const int CTRL_H = 44;
static const int CTRL_W = 120;
static const int TABLE_TOP = 160;
static const int TABLE_H = 520;
static const int ROW_H = 34;
static const int GAP = 16;

MasterDataWindow::MasterDataWindow() {

}
MasterDataWindow::~MasterDataWindow() {

}

void MasterDataWindow::Init() {
    if (initialized_) return;

    float totalWidth = (float)(SCREEN_W - PADDING * 2);
    float colWidth = totalWidth / 3.0f;

    // Define column rectangles
    walletCol_ = { (float)PADDING, (float)TABLE_TOP, colWidth - GAP / 2.0f, (float)TABLE_H };
    sourceCol_ = { walletCol_.x + colWidth, (float)TABLE_TOP, colWidth - GAP / 2.0f, (float)TABLE_H / 2.0f - GAP / 2.0f };
    categoryCol_ = { walletCol_.x + colWidth, sourceCol_.y + sourceCol_.height + GAP, colWidth - GAP / 2.0f, (float)TABLE_H / 2.0f - GAP / 2.0f };
    txnCol_ = { walletCol_.x + colWidth * 2, (float)TABLE_TOP, colWidth - GAP / 2.0f, (float)TABLE_H };

    // Initialize scroll areas
    walletArea_.Init(walletCol_.x, walletCol_.y, walletCol_.width, walletCol_.height);
    sourceArea_.Init(sourceCol_.x, sourceCol_.y, sourceCol_.width, sourceCol_.height);
    categoryArea_.Init(categoryCol_.x, categoryCol_.y, categoryCol_.width, categoryCol_.height);
    txnArea_.Init(txnCol_.x, txnCol_.y, txnCol_.width, txnCol_.height);

    // Initialize buttons
    backBtn_ = Button((float)PADDING, 18.0f, 140.0f, (float)CTRL_H, "< Dashboard",
                      LIGHTGRAY, GRAY, BLACK, [this]() {
                          if (onBackRequested_) onBackRequested_();
                      }, 18);


    float btnY = 18.0f;
    float addX = (float)(PADDING + 830);
    addBtn_ = Button(addX, btnY, (float)CTRL_W, (float)CTRL_H, "Add", GREEN, DARKGREEN, WHITE, [this]() {
        Focus target = (focus_ == Focus::None) ? Focus::Wallet : focus_;
        
        FormMode mode;
        if (target == Focus::Wallet) mode = FormMode::AddWallet;
        else if (target == Focus::Source) mode = FormMode::AddSource;
        else mode = FormMode::AddCategory;
        
        OpenFormDialog(mode, target);
    }, 18);

    editBtn_ = Button(addX + CTRL_W + 10, btnY, (float)CTRL_W, (float)CTRL_H, "Edit", SKYBLUE, BLUE, WHITE, [this]() {
        if (focus_ == Focus::Wallet && selectedWalletId_ < 0) { return; }
        if (focus_ == Focus::Source && selectedSourceId_ < 0) { return; }
        if (focus_ == Focus::Category && selectedCategoryId_ < 0) { return; }
        Focus target = (focus_ == Focus::None) ? Focus::Wallet : focus_;
        
        FormMode mode;
        if (target == Focus::Wallet) mode = FormMode::EditWallet;
        else if (target == Focus::Source) mode = FormMode::EditSource;
        else mode = FormMode::EditCategory;
        
        OpenFormDialog(mode, target);
    }, 18);

    deleteBtn_ = Button(addX + (CTRL_W + 10) * 2, btnY, (float)CTRL_W, (float)CTRL_H, "Delete", RED, MAROON, WHITE, [this]() {
        OpenDeleteConfirm();
    }, 18);

    initialized_ = true;
}

void MasterDataWindow::Update() {
    if (!initialized_) Init();

    // Only update buttons and scroll areas if no dialog is open
    if (!showFormDialog_ && !showConfirmDialog_) {
        backBtn_.Update();
        addBtn_.Update();
        editBtn_.Update();
        deleteBtn_.Update();

        // Column updates (scroll)
        walletArea_.Update(walletContentH_);
        sourceArea_.Update(sourceContentH_);
        categoryArea_.Update(categoryContentH_);
        txnArea_.Update(txnContentH_);
    }

    if (showFormDialog_) formDialog_.Update();
    if (showConfirmDialog_) confirmDialog_.Update();
}

void MasterDataWindow::Draw(DataManager& dataManager) {
    if (!initialized_) Init();
    dataManager_ = &dataManager;

    // Title bar
    Rectangle titleBox = { (float)PADDING, 10.0f, (float)(SCREEN_W - PADDING * 2), (float)TITLE_H };
    DrawRectangleRec(titleBox, Fade(SKYBLUE, 0.10f));
    DrawRectangleLinesEx(titleBox, 2.0f, DARKBLUE);
    LayoutHelper::MeasureAndDrawText("Master Data", 32, DARKBLUE, titleBox.x + PADDING + 150, titleBox.y + 18);

    backBtn_.Draw();
    addBtn_.Draw();
    editBtn_.Draw();
    deleteBtn_.Draw();

    // Status text hidden per request

    DrawColumns(dataManager);

    if (showFormDialog_) {
        formDialog_.Draw();
        // Draw input labels consistently (above each box)
        int idLabelY = (int)(formDialogRect_.y + 70 - 26);
        int nameLabelY = (int)(formDialogRect_.y + 150 - 26);
        DrawText("ID", (int)(formDialogRect_.x + 30), idLabelY, 20, DARKGRAY);
        DrawText("Name", (int)(formDialogRect_.x + 30), nameLabelY, 20, DARKGRAY);

        Rectangle AddButtonRect = { formDialogRect_.x + formDialogRect_.width - 200.0f, formDialogRect_.y + formDialogRect_.height - 70.0f, 80.0f, 44.0f }; 
        DrawFormErrorTextIndicator(AddButtonRect, errorID_);
    }
    if (showConfirmDialog_) {
        confirmDialog_.Draw();
    }
}

void MasterDataWindow::DrawColumns(DataManager& dataManager) {
    // Column titles (clickable to pick Add target)
    Vector2 mouse = GetMousePosition();

    Rectangle walletTitleBox   = { walletCol_.x,   walletCol_.y   - 32, walletCol_.width, 26 };
    Rectangle sourceTitleBox   = { sourceCol_.x,   sourceCol_.y   - 32, sourceCol_.width, 26 };
    Rectangle categoryTitleBox = { categoryCol_.x, categoryCol_.y - 32, categoryCol_.width, 26 };
    Rectangle txnTitleBox      = { txnCol_.x,      txnCol_.y      - 32, txnCol_.width, 26 };

    auto drawTitle = [&](const char* text, Rectangle box, Focus target, Color base) {
        bool hover = CheckCollisionPointRec(mouse, box);
        if (hover) DrawRectangleRec(box, Fade(LIGHTGRAY, 0.35f));
        LayoutHelper::MeasureAndDrawText(text, 20, base, box.x, box.y + 6);
        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Ensure exclusive focus between wallet and source/category
            focus_ = target;
            if (target == Focus::Wallet) {
                selectedSourceId_ = -1;
                selectedCategoryId_ = -1;
            } else {
                selectedWalletId_ = -1;
                if (target == Focus::Source) selectedCategoryId_ = -1;
                if (target == Focus::Category) selectedSourceId_ = -1;
            }
        }
    };

    drawTitle("Wallets", walletTitleBox, Focus::Wallet, DARKGRAY);
    drawTitle("Sources", sourceTitleBox, Focus::Source, DARKGRAY);
    drawTitle("Categories", categoryTitleBox, Focus::Category, DARKGRAY);
    drawTitle("Transactions", txnTitleBox, Focus::None, DARKGRAY);

    //Draw sepearators
    DrawLineEx({walletCol_.x + walletCol_.width + GAP / 4.0f, (float)TABLE_TOP - 20.0f},
               {walletCol_.x + walletCol_.width + GAP / 4.0f, (float)(TABLE_TOP + TABLE_H + 8)}, 2.0f, LIGHTGRAY);
    DrawLineEx({sourceCol_.x + sourceCol_.width + GAP / 4.0f, (float)TABLE_TOP - 20.0f},
                {sourceCol_.x + sourceCol_.width + GAP / 4.0f, (float)(TABLE_TOP + TABLE_H + 8)}, 2.0f, LIGHTGRAY);

    DrawWallets();
    DrawSourcesAndCategories();
    DrawTransactions();
}

void MasterDataWindow::DrawWallets() {
    int count = 0;
    int* ids = dataManager_->wallets_.getAllIDs(count);
    walletContentH_ = (float)((count + 1) * ROW_H);
    walletArea_.Update(walletContentH_);

    walletArea_.Begin();
    float y = walletArea_.view.y - walletArea_.GetOffset();
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < count; ++i) {
        Rectangle row = { walletArea_.view.x, y, walletArea_.view.width - 12.0f, (float)ROW_H - 4.0f };
        bool hovered = CheckCollisionPointRec(mouse, row);
        bool selected = ids[i] == selectedWalletId_;
        if (selected) DrawRectangleRec(row, Fade(SKYBLUE, 0.15f));
        else if (hovered) DrawRectangleRec(row, Fade(LIGHTGRAY, 0.6f));

        std::string name = dataManager_->wallets_.getName(ids[i]);
        LayoutHelper::MeasureAndDrawText(name, 18, BLACK, row.x + 8, y + 8);

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedWalletId_ = ids[i];
            focus_ = Focus::Wallet;
            ClearSelectionsBelowWallet();
        }
        y += ROW_H;
    }

    walletArea_.End();
    delete[] ids;
}

void MasterDataWindow::DrawSourcesAndCategories() {
    Vector2 mouse = GetMousePosition();

    sourceContentH_ = 0.0f;
    categoryContentH_ = 0.0f;

    // Sources (upper half) - always visible
    int sc = 0;
    int* srcIds = dataManager_->sources_.getAllIDs(sc);
    sourceContentH_ = (float)((sc + 1) * ROW_H);
    sourceArea_.Update(sourceContentH_);
    sourceArea_.Begin();
    float y = sourceArea_.view.y - sourceArea_.GetOffset();
    for (int i = 0; i < sc; ++i) {
        Rectangle row = { sourceArea_.view.x, y, sourceArea_.view.width - 12.0f, (float)ROW_H - 4.0f };
        bool hovered = CheckCollisionPointRec(mouse, row);
        bool selected = srcIds[i] == selectedSourceId_ && focus_ == Focus::Source;
        if (selected) DrawRectangleRec(row, Fade(SKYBLUE, 0.15f));
        else if (hovered) DrawRectangleRec(row, Fade(LIGHTGRAY, 0.6f));

        std::string name = dataManager_->sources_.getSourceName(srcIds[i]);
        LayoutHelper::MeasureAndDrawText(name, 18, BLACK, row.x + 8, y + 8);

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedSourceId_ = srcIds[i];
            selectedCategoryId_ = -1;
            selectedWalletId_ = -1; // exclusive focus
            focus_ = Focus::Source;
        }
        y += ROW_H;
    }
    sourceArea_.End();
    delete[] srcIds;

    // Categories (lower half) - always visible
    int cc = 0; // category count
    int* catIds = dataManager_->categories_.getAllIDs(cc); // category IDs

    categoryContentH_ = (float)((cc + 1) * ROW_H); // total content height
    // Update and draw category area
    categoryArea_.Update(categoryContentH_);
    categoryArea_.Begin();


    float y2 = categoryArea_.view.y - categoryArea_.GetOffset();
    for (int i = 0; i < cc; ++i) {
        Rectangle row = { categoryArea_.view.x, y2, categoryArea_.view.width - 12.0f, (float)ROW_H - 4.0f };
        bool hovered = CheckCollisionPointRec(mouse, row);
        bool selected = catIds[i] == selectedCategoryId_ && focus_ == Focus::Category;
        if (selected) DrawRectangleRec(row, Fade(SKYBLUE, 0.15f));
        else if (hovered) DrawRectangleRec(row, Fade(LIGHTGRAY, 0.6f));

        std::string name = dataManager_->categories_.getCategoryName(catIds[i]);
        LayoutHelper::MeasureAndDrawText(name, 18, BLACK, row.x + 8, y2 + 8);

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedCategoryId_ = catIds[i];
            selectedSourceId_ = -1;
            selectedWalletId_ = -1; // exclusive focus
            focus_ = Focus::Category;
        }
        y2 += ROW_H;
    }
    categoryArea_.End();
    delete[] catIds;
}

void MasterDataWindow::DrawTransactions() {
    txnArea_.Update(txnContentH_);
    txnArea_.Begin();

    float y = txnArea_.view.y - txnArea_.GetOffset();
    Vector2 mouse = GetMousePosition();
    int rows = 0;

    if (focus_ == Focus::Wallet && selectedWalletId_ >= 0) {
        int inCount = dataManager_->incomes_.getCount();
        for (int i = 0; i < inCount; ++i) {
            IncomeTransaction it = dataManager_->incomes_.getAt(i);
            if (it.walletID != selectedWalletId_) continue;
            Rectangle row = { txnArea_.view.x, y, txnArea_.view.width - 12.0f, (float)ROW_H - 4.0f };
            if (CheckCollisionPointRec(mouse, row)) DrawRectangleRec(row, Fade(SKYBLUE, 0.10f));
            std::string line = "[Income] +" + FormatCurrency((long long)it.amount) + "  -  " + it.description;
            LayoutHelper::MeasureAndDrawText(line, 18, DARKGREEN, row.x + 8, y + 8);
            y += ROW_H;
            rows++;
        }

        int exCount = dataManager_->expenses_.getCount();
        for (int i = 0; i < exCount; ++i) {
            ExpenseTransaction et = dataManager_->expenses_.getAt(i);
            if (et.walletID != selectedWalletId_) continue;
            Rectangle row = { txnArea_.view.x, y, txnArea_.view.width - 12.0f, (float)ROW_H - 4.0f };
            if (CheckCollisionPointRec(mouse, row)) DrawRectangleRec(row, Fade(SKYBLUE, 0.10f));
            std::string line = "[Expense] -" + FormatCurrency((long long)et.amount) + "  -  " + et.description;
            LayoutHelper::MeasureAndDrawText(line, 18, MAROON, row.x + 8, y + 8);
            y += ROW_H;
            rows++;
        }

        if (rows == 0) {
            LayoutHelper::MeasureAndDrawText("No transactions for this wallet", 18, GRAY, txnArea_.view.x, txnArea_.view.y);
        }
    } else if (focus_ == Focus::Source && selectedSourceId_ >= 0) {
        int count = dataManager_->incomes_.getCount();
        for (int i = 0; i < count; ++i) {
            IncomeTransaction it = dataManager_->incomes_.getAt(i);
            if (it.sourceID != selectedSourceId_) continue;
            Rectangle row = { txnArea_.view.x, y, txnArea_.view.width - 12.0f, (float)ROW_H - 4.0f };
            if (CheckCollisionPointRec(mouse, row)) DrawRectangleRec(row, Fade(SKYBLUE, 0.10f));
            std::string line = FormatCurrency((long long)it.amount) + "  -  " + it.description;
            LayoutHelper::MeasureAndDrawText(line, 18, DARKGREEN, row.x + 8, y + 8);
            y += ROW_H;
            rows++;
        }
    } else if (focus_ == Focus::Category && selectedCategoryId_ >= 0) {
        int count = dataManager_->expenses_.getCount();
        for (int i = 0; i < count; ++i) {
            ExpenseTransaction et = dataManager_->expenses_.getAt(i);
            if (et.categoryID != selectedCategoryId_) continue;
            Rectangle row = { txnArea_.view.x, y, txnArea_.view.width - 12.0f, (float)ROW_H - 4.0f };
            if (CheckCollisionPointRec(mouse, row)) DrawRectangleRec(row, Fade(SKYBLUE, 0.10f));
            std::string line = FormatCurrency((long long)et.amount) + "  -  " + et.description;
            LayoutHelper::MeasureAndDrawText(line, 18, MAROON, row.x + 8, y + 8);
            y += ROW_H;
            rows++;
        }
    } else {
        LayoutHelper::MeasureAndDrawText("Select wallet/source/category to view transactions", 18, GRAY, txnArea_.view.x, txnArea_.view.y);
    }

    txnContentH_ = (float)((rows + 1) * ROW_H);
    txnArea_.End();
}

void MasterDataWindow::ClearSelectionsBelowWallet() {
    selectedSourceId_ = -1;
    selectedCategoryId_ = -1;
    focus_ = Focus::Wallet;
}

// Generate next available ID for a NameTable
int MasterDataWindow::NextId(NameTable& table) const {
    int c = 0;
    int* ids = table.getAllIDs(c);
    int next = 1;
    for (int i = 0; i < c; ++i) {
        if (ids[i] >= next) next = ids[i] + 1;
    }
    delete[] ids;
    return next;
}

void MasterDataWindow::OpenFormDialog(FormMode mode, Focus target) {
    if (!dataManager_) { return; }

    formMode_ = mode;
    formTarget_ = target;
    editOriginalId_ = -1;

    errorID_ = -1;

    Rectangle r = LayoutHelper::CenterRect(0, 0, (float)SCREEN_W, (float)SCREEN_H, 520.0f, 260.0f);
    formDialogRect_ = r;
    
    std::string title;
    if (mode == FormMode::AddWallet) title = "Add Wallet";
    else if (mode == FormMode::AddSource) title = "Add Source";
    else if (mode == FormMode::AddCategory) title = "Add Category";
    else if (mode == FormMode::EditWallet) title = "Edit Wallet";
    else if (mode == FormMode::EditSource) title = "Edit Source";
    else if (mode == FormMode::EditCategory) title = "Edit Category";
    
    formDialog_ = Dialog(r.x, r.y, r.width, r.height, title);

    DrawText("ID: ", r.x + 30, r.y + 50, 24, BLACK);
    formDialog_.AddTextInput(30, 70, 160, 36, "ID");
    DrawText("Name: ", r.x + 30, r.y + 130, 24, BLACK);
    formDialog_.AddTextInput(30, 150, 360, 36, "Name");
    // Character caps for inputs
    formDialog_.SetInputMaxLength(0, 10);  // ID
    formDialog_.SetInputMaxLength(1, 40);  // Name
    
    bool isAdd = (mode == FormMode::AddWallet || mode == FormMode::AddSource || mode == FormMode::AddCategory);
    formDialog_.AddButton(r.width - 200, r.height - 70, 80, 44, isAdd ? "Add" : "Save",
                          GREEN, DARKGREEN, WHITE, [this]() { SubmitForm(); });
    formDialog_.AddButton(r.width - 110, r.height - 70, 80, 44, "Cancel",
                          RED, MAROON, WHITE, [this]() { CloseDialogs(); });

    if (isAdd) {
        int nextId = 1;
        if (target == Focus::Wallet) nextId = NextId(dataManager_->wallets_);
        else if (target == Focus::Source) nextId = NextId(dataManager_->sources_);
        else if (target == Focus::Category) nextId = NextId(dataManager_->categories_);
        formDialog_.SetInputText(0, std::to_string(nextId));
    } else {
        int currentId = -1;
        std::string currentName;
        if (target == Focus::Wallet && selectedWalletId_ >= 0) {
            currentId = selectedWalletId_;
            currentName = dataManager_->wallets_.getName(currentId);
        } else if (target == Focus::Source && selectedSourceId_ >= 0) {
            currentId = selectedSourceId_;
            currentName = dataManager_->sources_.getSourceName(currentId);
        } else if (target == Focus::Category && selectedCategoryId_ >= 0) {
            currentId = selectedCategoryId_;
            currentName = dataManager_->categories_.getCategoryName(currentId);
        } else {
            return;
        }
        editOriginalId_ = currentId;
        formDialog_.SetInputText(0, std::to_string(currentId));
        formDialog_.SetInputText(1, currentName);
    }

    showFormDialog_ = true;
    formDialog_.Open();
}

void MasterDataWindow::SubmitForm() {
    if (!dataManager_) { return; }
    std::string idText = formDialog_.GetInputText(0);
    std::string nameText = formDialog_.GetInputText(1);

    // Validate name
    if (nameText.empty()) { errorID_  = 7; return; }

    // Check ID validity
    int parsedId = -1;
    try { parsedId = std::stoi(idText); } catch (...) {errorID_ = 8; return; }
    if (parsedId <= 0) { errorID_ = 8; return; }

    // Check for ID uniqueness when adding
    auto idExists = [&](NameTable& tbl, int id) {
        int c = 0; int* ids = tbl.getAllIDs(c);
        bool exists = false;
        for (int i = 0; i < c; ++i) { if (ids[i] == id) { exists = true; break; } }
        delete[] ids;
        return exists;
    };

    bool isEdit = (formMode_ == FormMode::EditWallet || formMode_ == FormMode::EditSource || formMode_ == FormMode::EditCategory);
    int targetId = parsedId;
    if (isEdit) targetId = editOriginalId_; // keep ID stable to avoid breaking references


    switch (formTarget_) {
    case Focus::Wallet: {
        if (!isEdit && idExists(dataManager_->wallets_, targetId)) { errorID_ = 8; return; }
        if (isEdit) {
            dataManager_->wallets_.editName(targetId, nameText);
        } else {
            dataManager_->wallets_.addWallet(targetId, nameText);
        }
        dataManager_->SaveAllData();
        break;
    }
    case Focus::Source: {
        if (!isEdit && idExists(dataManager_->sources_, targetId)) { errorID_ = 8; return; }
        if (isEdit) {
            dataManager_->sources_.editName(targetId, nameText);
        } else {
            dataManager_->sources_.insertSource(targetId, nameText);
        }
        dataManager_->SaveAllData();
        break;
    }
    case Focus::Category: {
        if (!isEdit && idExists(dataManager_->categories_, targetId)) { errorID_ = 8; return; }
        if (isEdit) {
            dataManager_->categories_.editName(targetId, nameText);
        } else {
            dataManager_->categories_.addCategory(targetId, nameText);
        }
        dataManager_->SaveAllData();
        break;
    }
    default:
        return;
    }

    errorID_ = -1;
    CloseDialogs();
}

void MasterDataWindow::OpenDeleteConfirm() {
    if (!dataManager_) { return; }

    int id = -1;
    const char* targetName = "Item";

    if (focus_ == Focus::Wallet && selectedWalletId_ >= 0) { id = selectedWalletId_; targetName = "Wallet"; }
    else if (focus_ == Focus::Source && selectedSourceId_ >= 0) { id = selectedSourceId_; targetName = "Source"; }
    else if (focus_ == Focus::Category && selectedCategoryId_ >= 0) { id = selectedCategoryId_; targetName = "Category"; }
    else { return; }

    Rectangle r = LayoutHelper::CenterRect(0, 0, (float)SCREEN_W, (float)SCREEN_H, 440.0f, 220.0f);
    confirmDialog_ = Dialog(r.x, r.y, r.width, r.height, "Confirm delete");
    // Bigger centered buttons
    float btnW = 140.0f;
    float btnH = 52.0f;
    float spacing = 24.0f;
    float totalW = btnW * 2.0f + spacing;
    float startX = (r.width - totalW) / 2.0f;
    float y = r.height - 80.0f;
    confirmDialog_.AddButton(startX, y, btnW, btnH, "Yes",
                             RED, MAROON, WHITE, [this]() { ConfirmDelete(); });
    confirmDialog_.AddButton(startX + btnW + spacing, y, btnW, btnH, "No",
                             GREEN, DARKGREEN, WHITE, [this]() { CloseDialogs(); });
    formTarget_ = focus_;
    showConfirmDialog_ = true;
    confirmDialog_.Open();
}

void MasterDataWindow::ConfirmDelete() {
    if (!dataManager_) { return; }

    switch (formTarget_) {
    case Focus::Wallet:
        if (selectedWalletId_ < 0) { break; }
        dataManager_->wallets_.remove(selectedWalletId_);
        selectedWalletId_ = -1;
        ClearSelectionsBelowWallet();
        dataManager_->SaveAllData();
        break;
    case Focus::Source:
        if (selectedSourceId_ < 0) { break; }
        dataManager_->sources_.remove(selectedSourceId_);
        selectedSourceId_ = -1;
        dataManager_->SaveAllData();
        break;
    case Focus::Category:
        if (selectedCategoryId_ < 0) { break; }
        dataManager_->categories_.remove(selectedCategoryId_);
        selectedCategoryId_ = -1;
        dataManager_->SaveAllData();
        break;
    default:
        break;
    }

    CloseDialogs();
}

void MasterDataWindow::CloseDialogs() {
    showFormDialog_ = false;
    showConfirmDialog_ = false;
    errorID_ = -1;
    formDialog_.Close();
    confirmDialog_.Close();
}


