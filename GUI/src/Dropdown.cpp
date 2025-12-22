#include "Dropdown.h"

Dropdown::Dropdown(float startx, float starty, float width, float height, const std::string& placeholder)
        : rect_{startx, starty, width, height}, placeholder_(placeholder), isOpen_(false),
            labels_(nullptr), values_(nullptr), count_(0), capacity_(0),
            selectedValue_(-1), selectedLabel_("") {
        // Initialize scroll area just below the main button; height will be adjusted dynamically
        float listHeight = (float)(MAX_VISIBLE_ITEMS * ITEM_HEIGHT);
        listScroll_.Init(rect_.x, rect_.y + rect_.height + 2.0f, rect_.width, listHeight);
}

Dropdown::~Dropdown() {
    delete[] labels_;
    delete[] values_;
}

void Dropdown::AddOption(const std::string& label, int value) {
    // Resize if needed
    if (count_ == capacity_) {
        int newCap = capacity_ == 0 ? 4 : capacity_ * 2;
        std::string* newLabels = new std::string[newCap];
        int* newValues = new int[newCap];
        for (int i = 0; i < count_; ++i) {
            newLabels[i] = labels_[i];
            newValues[i] = values_[i];
        }
        delete[] labels_;
        delete[] values_;
        labels_ = newLabels;
        values_ = newValues;
        capacity_ = newCap;
    }


    // Add new option
    labels_[count_] = label;
    values_[count_] = value;
    count_++;
}

// Set selected option by value
void Dropdown::SetSelected(int value) {
    for (int i = 0; i < count_; ++i) {
        if (values_[i] == value) {
            selectedValue_ = value;
            selectedLabel_ = labels_[i];
            return;
        }
    }
}

// Get selected label or placeholder if none selected
std::string Dropdown::GetSelectedLabel() const {
    return selectedLabel_.empty() ? placeholder_ : selectedLabel_;
}

// Update dropdown state (handle input)
void Dropdown::Update() {
    Vector2 mouse = GetMousePosition();

    // Check if main button is clicked
    if (CheckCollisionPointRec(mouse, rect_) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isOpen_ = !isOpen_;
    }

    // If dropdown is open, update scroll and check for option clicks
    if (isOpen_) {
        // Keep the scroll view anchored below the button
        float listHeight = (float)(MAX_VISIBLE_ITEMS * ITEM_HEIGHT);
        listScroll_.view = { rect_.x, rect_.y + rect_.height + 2.0f, rect_.width, listHeight };

        float contentH = (float)(count_ * ITEM_HEIGHT);
        listScroll_.Update(contentH);

        float baseY = listScroll_.view.y - listScroll_.GetOffset();

        for (int i = 0; i < count_; ++i) {
            Rectangle optionRect = { rect_.x, baseY + (float)(i * ITEM_HEIGHT), rect_.width, (float)ITEM_HEIGHT };

            if (CheckCollisionPointRec(mouse, optionRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Only consider clicks within the visible scissor region
                if (optionRect.y + optionRect.height >= listScroll_.view.y && optionRect.y <= listScroll_.view.y + listScroll_.view.height) {
                    
                    // Update selection
                    selectedValue_ = values_[i];
                    selectedLabel_ = labels_[i];

                    // Close dropdown after selection
                    isOpen_ = false;

                    break;
                }
            }
        }
    }
}

// Draw the dropdown
void Dropdown::Draw() {
    
    // Draw main button
    Color buttonColor = isOpen_ ? LIGHTGRAY : WHITE;
    DrawRectangleRec(rect_, buttonColor);
    DrawRectangleLinesEx(rect_, 2.0f, BLACK);

    // Draw selected text or placeholder
    std::string displayText = selectedLabel_.empty() ? placeholder_ : selectedLabel_;
    DrawText(displayText.c_str(), (int)rect_.x + 8, (int)rect_.y + 8, 18, BLACK);

    // Draw dropdown arrow (▼)
    DrawText("v", (int)(rect_.x + rect_.width - 20), (int)rect_.y + 8, 18, DARKGRAY);

    // Draw dropdown list if open
    if (isOpen_) {
        // Background for the list (clipped by scissor inside Begin/End)
        Rectangle listBg = listScroll_.view;
        DrawRectangleRec(listBg, Fade(LIGHTGRAY, 0.95f));
        DrawRectangleLinesEx(listBg, 2.0f, DARKGRAY);

        // Draw options within scroll area
        listScroll_.Begin();
        float baseY = listScroll_.view.y - listScroll_.GetOffset();
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < count_; ++i) {
            Rectangle optionRect = { rect_.x, baseY + (float)(i * ITEM_HEIGHT), rect_.width, (float)ITEM_HEIGHT };
            bool isHovered = CheckCollisionPointRec(mouse, optionRect);
            DrawOption(labels_[i], optionRect.x, optionRect.y, optionRect.width, optionRect.height, isHovered);
        }
        listScroll_.End();
    }
}

// Helper to draw a single option
void Dropdown::DrawOption(const std::string& label, float x, float y, float w, float h, bool isHovered) {
    // Highlight on hover
    if (isHovered) {
        DrawRectangle((int)x, (int)y, (int)w, (int)h, Fade(SKYBLUE, 0.3f));
    }

    // Draw option text
    DrawText(label.c_str(), (int)x + 8, (int)y + 6, 18, BLACK);

    // Draw bottom border line
    DrawLine((int)x, (int)(y + h), (int)(x + w), (int)(y + h), LIGHTGRAY);
}
