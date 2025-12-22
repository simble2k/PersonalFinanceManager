#include "../include/TextInput.h"

#include <algorithm>

TextInput::TextInput() : rect_{0, 0, 0, 0}, placeholder_(""), cursorPos_(0), isActive_(false) {}

TextInput::TextInput(float x, float y, float width, float height, const std::string& placeholder)
    : rect_{x, y, width, height}, placeholder_(placeholder), cursorPos_(0), isActive_(false) {}

void TextInput::Update() {
    Vector2 mouse = GetMousePosition();
    bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    // IsActive when clicked inside and inActive when clicked outside the box
    if (CheckCollisionPointRec(mouse, rect_)) {
        if (clicked) {
            isActive_ = true;
        }
    } else if (clicked) {
        isActive_ = false;
    }

    // Handle text input when active
    if (isActive_) {
        int key = GetCharPressed(); //Get keyboard input
        while (key > 0) {
            if (key >= 32 && key <= 125) {  // Accept printable characters
                text_.insert(cursorPos_, 1, (char)key); // Add char to "text_" string at cursor position
                cursorPos_++;
            }
            key = GetCharPressed();
        }

        // Handle delete, backspace, and arrow keys logic
        if (IsKeyPressed(KEY_BACKSPACE) && cursorPos_ > 0) {
            text_.erase(cursorPos_ - 1, 1);
            cursorPos_--;
        }
        if (IsKeyPressed(KEY_DELETE) && cursorPos_ < (int)text_.size()) {
            text_.erase(cursorPos_, 1);
        }
        if (IsKeyPressed(KEY_LEFT) && cursorPos_ > 0) {
            cursorPos_--;
        }
        if (IsKeyPressed(KEY_RIGHT) && cursorPos_ < (int)text_.size()) {
            cursorPos_++;
        }
    }
}

void TextInput::Draw() { // Draw text box
    //Change color when active
    Color bgColor = isActive_ ? LIGHTGRAY : GRAY;

    //Draw box
    DrawRectangleRec(rect_, bgColor);
    DrawRectangleLinesEx(rect_, 2.0f, isActive_ ? BLUE : DARKGRAY); //box border

    // Display text or placeholder
    std::string displayText = text_.empty() ? placeholder_ : text_;
    Color textColor = text_.empty() ? GRAY : BLACK;
    DrawText(displayText.c_str(), rect_.x + 5, rect_.y + 5, 20, textColor);

    if (isActive_) {
        // Draw cursor if active
        int cursorX = rect_.x + 5 + MeasureText(text_.substr(0, cursorPos_).c_str(), 20);
        DrawLine(cursorX, rect_.y + 5, cursorX, rect_.y + rect_.height - 5, BLACK);
    }
}