#include "MasterDataWindow.h"
#include "NameTable.h"
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
        OpenFormDialog(FormMode::Add, target);
    }, 18);

    editBtn_ = Button(addX + CTRL_W + 10, btnY, (float)CTRL_W, (float)CTRL_H, "Edit", SKYBLUE, BLUE, WHITE, [this]() {
        if (focus_ == Focus::Wallet && selectedWalletId_ < 0) { SetStatus("Select a wallet", RED); return; }
        if (focus_ == Focus::Source && selectedSourceId_ < 0) { SetStatus("Select a source", RED); return; }
        if (focus_ == Focus::Category && selectedCategoryId_ < 0) { SetStatus("Select a category", RED); return; }
        Focus target = (focus_ == Focus::None) ? Focus::Wallet : focus_;
        OpenFormDialog(FormMode::Edit, target);
    }, 18);

    deleteBtn_ = Button(addX + (CTRL_W + 10) * 2, btnY, (float)CTRL_W, (float)CTRL_H, "Delete", RED, MAROON, WHITE, [this]() {
        OpenDeleteConfirm();
    }, 18);

    initialized_ = true;
}

void MasterDataWindow::Update() {
    if (!initialized_) Init();

    backBtn_.Update();
    addBtn_.Update();
    editBtn_.Update();
    deleteBtn_.Update();

    // Column updates (scroll)
    walletArea_.Update(walletContentH_);
    sourceArea_.Update(sourceContentH_);
    categoryArea_.Update(categoryContentH_);
    txnArea_.Update(txnContentH_);

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

    if (!status_.empty()) {
        DrawText(status_.c_str(), (int)(titleBox.x + PADDING), (int)(titleBox.y + TITLE_H - 18), 18, statusColor_);
    }

    DrawColumns(dataManager);

    if (showFormDialog_) {
        formDialog_.Draw();
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
            focus_ = target;
            SetStatus("Add targets " + std::string(text), DARKGRAY);
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
            SetStatus("Wallet selected", DARKGRAY);
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

    // Sources (upper half)
    if (selectedWalletId_ < 0) {
        LayoutHelper::MeasureAndDrawText("Select a wallet", 18, GRAY, sourceCol_.x, sourceCol_.y + 4);
    } else {
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
                focus_ = Focus::Source;
                SetStatus("Income source selected", DARKGRAY);
            }
            y += ROW_H;
        }
        sourceArea_.End();
        delete[] srcIds;
    }

    // Categories (lower half)
    if (selectedWalletId_ < 0) {
        LayoutHelper::MeasureAndDrawText("", 0, GRAY, 0, 0); // nothing
    } else {
        int cc = 0;
        int* catIds = dataManager_->categories_.getAllIDs(cc);
        categoryContentH_ = (float)((cc + 1) * ROW_H);
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
                focus_ = Focus::Category;
                SetStatus("Category selected", DARKGRAY);
            }
            y2 += ROW_H;
        }
        categoryArea_.End();
        delete[] catIds;
    }
}

void MasterDataWindow::DrawTransactions() {
    txnArea_.Update(txnContentH_);
    txnArea_.Begin();

    float y = txnArea_.view.y - txnArea_.GetOffset();
    Vector2 mouse = GetMousePosition();
    int rows = 0;

    if (focus_ == Focus::Source && selectedSourceId_ >= 0) {
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
        LayoutHelper::MeasureAndDrawText("Select source/category to view transactions", 18, GRAY, txnArea_.view.x, txnArea_.view.y);
    }

    txnContentH_ = (float)((rows + 1) * ROW_H);
    txnArea_.End();
}

void MasterDataWindow::ClearSelectionsBelowWallet() {
    selectedSourceId_ = -1;
    selectedCategoryId_ = -1;
    focus_ = Focus::Wallet;
}

// Set status message with color
void MasterDataWindow::SetStatus(const std::string& msg, Color color) {
    status_ = msg;
    statusColor_ = color;
}

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
    if (!dataManager_) { SetStatus("No data manager", RED); return; }

    formMode_ = mode;
    formTarget_ = target;
    editOriginalId_ = -1;

    Rectangle r = LayoutHelper::CenterRect(0, 0, (float)SCREEN_W, (float)SCREEN_H, 520.0f, 260.0f);
    const char* title = (mode == FormMode::Add) ? "Add" : "Edit";
    formDialog_ = Dialog(r.x, r.y, r.width, r.height, title);

    DrawText("ID: ", r.x + 30, r.y + 50, 24, BLACK);
    formDialog_.AddTextInput(30, 70, 160, 36, "ID");
    DrawText("Name: ", r.x + 30, r.y + 130, 24, BLACK);
    formDialog_.AddTextInput(30, 150, 360, 36, "Name");
    formDialog_.AddButton(r.width - 200, r.height - 70, 80, 44, (mode == FormMode::Add) ? "Add" : "Save",
                          GREEN, DARKGREEN, WHITE, [this]() { SubmitForm(); });
    formDialog_.AddButton(r.width - 110, r.height - 70, 80, 44, "Cancel",
                          RED, MAROON, WHITE, [this]() { CloseDialogs(); });

    if (mode == FormMode::Add) {
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
            SetStatus("Select an item to edit", RED);
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
    if (!dataManager_) { SetStatus("No data manager", RED); return; }
    std::string idText = formDialog_.GetInputText(0);
    std::string nameText = formDialog_.GetInputText(1);

    if (nameText.empty()) { SetStatus("Name required", RED); return; }

    int parsedId = -1;
    try { parsedId = std::stoi(idText); } catch (...) { SetStatus("Invalid ID", RED); return; }
    if (parsedId <= 0) { SetStatus("ID must be > 0", RED); return; }

    auto idExists = [&](NameTable& tbl, int id) {
        int c = 0; int* ids = tbl.getAllIDs(c);
        bool exists = false;
        for (int i = 0; i < c; ++i) { if (ids[i] == id) { exists = true; break; } }
        delete[] ids;
        return exists;
    };

    bool isEdit = (formMode_ == FormMode::Edit);
    int targetId = parsedId;
    if (isEdit) targetId = editOriginalId_; // keep ID stable to avoid breaking references

    switch (formTarget_) {
    case Focus::Wallet: {
        if (!isEdit && idExists(dataManager_->wallets_, targetId)) { SetStatus("ID exists", RED); return; }
        if (isEdit) {
            dataManager_->wallets_.editName(targetId, nameText);
        } else {
            dataManager_->wallets_.addWallet(targetId, nameText);
        }
        dataManager_->SaveAllData();
        SetStatus(isEdit ? "Wallet saved" : "Wallet added", DARKGREEN);
        break;
    }
    case Focus::Source: {
        if (!isEdit && idExists(dataManager_->sources_, targetId)) { SetStatus("ID exists", RED); return; }
        if (isEdit) {
            dataManager_->sources_.editName(targetId, nameText);
        } else {
            dataManager_->sources_.insertSource(targetId, nameText);
        }
        dataManager_->SaveAllData();
        SetStatus(isEdit ? "Source saved" : "Source added", DARKGREEN);
        break;
    }
    case Focus::Category: {
        if (!isEdit && idExists(dataManager_->categories_, targetId)) { SetStatus("ID exists", RED); return; }
        if (isEdit) {
            dataManager_->categories_.editName(targetId, nameText);
        } else {
            dataManager_->categories_.addCategory(targetId, nameText);
        }
        dataManager_->SaveAllData();
        SetStatus(isEdit ? "Category saved" : "Category added", DARKGREEN);
        break;
    }
    default:
        SetStatus("Select a target", RED);
        return;
    }

    CloseDialogs();
}

void MasterDataWindow::OpenDeleteConfirm() {
    if (!dataManager_) { SetStatus("No data manager", RED); return; }

    int id = -1;
    const char* targetName = "Item";

    if (focus_ == Focus::Wallet && selectedWalletId_ >= 0) { id = selectedWalletId_; targetName = "Wallet"; }
    else if (focus_ == Focus::Source && selectedSourceId_ >= 0) { id = selectedSourceId_; targetName = "Source"; }
    else if (focus_ == Focus::Category && selectedCategoryId_ >= 0) { id = selectedCategoryId_; targetName = "Category"; }
    else { SetStatus("Select an item to delete", RED); return; }

    Rectangle r = LayoutHelper::CenterRect(0, 0, (float)SCREEN_W, (float)SCREEN_H, 420.0f, 200.0f);
    confirmDialog_ = Dialog(r.x, r.y, r.width, r.height, "Confirm delete");
    confirmDialog_.AddButton(r.width - 200, r.height - 60, 80, 44, "Yes",
                             RED, MAROON, WHITE, [this]() { ConfirmDelete(); });
    confirmDialog_.AddButton(r.width - 110, r.height - 60, 80, 44, "No",
                             GREEN, DARKGREEN, WHITE, [this]() { CloseDialogs(); });
    formTarget_ = focus_;
    showConfirmDialog_ = true;
    confirmDialog_.Open();
    SetStatus(std::string("Delete ") + targetName + "?", DARKGRAY);
}

void MasterDataWindow::ConfirmDelete() {
    if (!dataManager_) { SetStatus("No data manager", RED); return; }

    switch (formTarget_) {
    case Focus::Wallet:
        if (selectedWalletId_ < 0) { SetStatus("Select a wallet", RED); break; }
        dataManager_->wallets_.remove(selectedWalletId_);
        selectedWalletId_ = -1;
        ClearSelectionsBelowWallet();
        dataManager_->SaveAllData();
        SetStatus("Wallet deleted", DARKGREEN);
        break;
    case Focus::Source:
        if (selectedSourceId_ < 0) { SetStatus("Select a source", RED); break; }
        dataManager_->sources_.remove(selectedSourceId_);
        selectedSourceId_ = -1;
        dataManager_->SaveAllData();
        SetStatus("Source deleted", DARKGREEN);
        break;
    case Focus::Category:
        if (selectedCategoryId_ < 0) { SetStatus("Select a category", RED); break; }
        dataManager_->categories_.remove(selectedCategoryId_);
        selectedCategoryId_ = -1;
        dataManager_->SaveAllData();
        SetStatus("Category deleted", DARKGREEN);
        break;
    default:
        SetStatus("Select a target", RED);
        break;
    }

    CloseDialogs();
}

void MasterDataWindow::CloseDialogs() {
    showFormDialog_ = false;
    showConfirmDialog_ = false;
    formDialog_.Close();
    confirmDialog_.Close();
}
