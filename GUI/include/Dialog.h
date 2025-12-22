#pragma once
// Dialog box or window for user input with text fields and buttons

#include "raylib.h"
#include "Button.h"
#include "TextInput.h"
#include <string>
#include <functional>

class Dialog {
public:
    Dialog();  // Default constructor
    Dialog(float x, float y, float width, float height, const std::string& title);
    ~Dialog();

    void AddTextInput(float startX, float startY, float w, float h, const std::string& placeholder);
    void AddButton(float startX, float startY, float w, float h, const std::string& text,
                  Color normalColor = BLUE, Color hoverColor = DARKBLUE, Color textColor = WHITE,
                  std::function<void()> onClick = nullptr);

    // Clear all text inputs back to empty
    void ResetInputs();

    // Set text of a specific input (by index)
    void SetInputText(int index, const std::string& text);

    Rectangle GetRect() const { return rect_; }

    void Update();
    void Draw();
    bool IsOpen() const { return isOpen_; }
    void Open() { isOpen_ = true; }
    void Close() { isOpen_ = false; }

    // Get text from a specific input (by index)
    std::string GetInputText(int index) const;

private:
    Rectangle rect_;
    std::string title_;
    bool isOpen_;

    TextInput* inputs_;
    int inputCount_;
    int inputCapacity_;

    Button* buttons_;
    int buttonCount_;
    int buttonCapacity_;
};