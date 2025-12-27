#pragma once
// Handles text input in the GUI

#include "raylib.h"
#include <string>

class TextInput {
public:
    TextInput(); // Default constructor
    TextInput(float x, float y, float width, float height, const std::string& placeholder = ""); // default placeholder is "" (can be set)

    void Update(); // Update input state
    void Draw();
    std::string GetText() const { return text_; }
    void SetText(const std::string& text) { text_ = text; }
    void Clear() { text_.clear(); cursorPos_ = 0; }
    void SetMaxLength(int maxLen) { maxLength_ = maxLen; }

private:
    Rectangle rect_; // text box

    std::string text_; // place to store current text
    std::string placeholder_; // placeholder text when empty

    int cursorPos_; // cursor position in text
    bool isActive_; // active when clicked

    // Maximum character count; negative means unlimited
    int maxLength_ = -1;
};