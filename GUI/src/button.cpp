#include "Button.h"

Button::Button()
        : rect_{0, 0, 0, 0}, text_(""), normalColor_(GRAY), hoverColor_(DARKGRAY),
            textColor_(BLACK), fontSize_(20), isHovered_(false), onClick_(nullptr) {}

Button::Button(float x, float y, float width, float height, const std::string& text,
                             Color normalColor, Color hoverColor, Color textColor,
                             std::function<void()> onClick, int fontSize)
        : rect_{x, y, width, height}, text_(text), normalColor_(normalColor),
            hoverColor_(hoverColor), textColor_(textColor), fontSize_(fontSize),
            isHovered_(false), onClick_(onClick) {} // Constructor

void Button::Update() {
    Vector2 mouse = GetMousePosition();
    isHovered_ = CheckCollisionPointRec(mouse, rect_);

    // If hovered and clicked , callback onClick
    if (isHovered_ && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && onClick_) {
        onClick_();
    }
}

// Draw the button
void Button::Draw() {
    //Change color when hovered
    Color color = isHovered_ ? hoverColor_ : normalColor_;

    // Draw rectangle and text
    DrawRectangleRec(rect_, color);
    DrawRectangleLinesEx(rect_, 2.0f, BLACK); // Border (change color if needed)
    int textWidth = MeasureText(text_.c_str(), fontSize_);
    DrawText(text_.c_str(), rect_.x + (rect_.width - textWidth) / 2,
             rect_.y + (rect_.height - fontSize_) / 2, fontSize_, textColor_);
}