#pragma once
// For constructing buttons in the GUI windows

#include "raylib.h"
#include <string>
#include <functional> // for std::function<void()>

class Button {
public:
    Button(); // Default constructor
    // The arguments required are : position (xstart, ystart), size (width, height),
                                // text to display, normal color, hover color, text color,
                                // optional font size (default 20) and optional onClick
    Button(float xstart, float ystart, float width, float height, const std::string& text,
           Color normalColor, Color hoverColor, Color textColor,
           std::function<void()> onClick = nullptr, int fontSize = 20); // When clicked call onClick to run and do nothing if null

    void Update();  // Check for hover/click
    void Draw();    // Render the button

private:
    Rectangle rect_;
    std::string text_;
    Color normalColor_, hoverColor_, textColor_;
    int fontSize_ = 20;
    bool isHovered_;
    std::function<void()> onClick_;
};