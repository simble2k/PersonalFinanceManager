#pragma once

#include "raylib.h"
#include "ScrollArea.h"
#include <string>

class Dropdown {
public:
    Dropdown(float startx, float starty, float width, float height, const std::string& placeholder = "Select...");
    ~Dropdown();

    // Add an option to the dropdown
    void AddOption(const std::string& label, int value);

    // Update and draw
    void Update();
    // Draw the control; call DrawBase() then DrawListOverlay() when you need overlay on top of other elements
    void Draw();
    void DrawBase();
    void DrawListOverlay();

    // Getters
    bool IsOpen() const { return isOpen_; }
    void Open() { isOpen_ = true; }
    void Close() { isOpen_ = false; }
    int GetSelectedValue() const { return selectedValue_; }
    std::string GetSelectedLabel() const;
    
    // Set selected by value
    void SetSelected(int value);

private:
    Rectangle rect_;           // Main dropdown button area
    std::string placeholder_;  // Text when nothing selected
    bool isOpen_;              // Is dropdown expanded?
    
    std::string* labels_;      // Option labels (dynamic array)
    int* values_;              // Option values (IDs) (dynamic array)
    
    int count_;                // Current number of options
    int capacity_;             // Capacity of arrays
    int selectedValue_;        // Currently selected value (-1 = none)
    std::string selectedLabel_; // Currently selected label
    
    // Dropdown list dimensions
    static const int ITEM_HEIGHT = 30;
    static const int MAX_VISIBLE_ITEMS = 5;  // Max items visible before scrolling
    
    // Helper function to draw an option
    void DrawOption(const std::string& label, float x, float y, float w, float h, bool isHovered);

    // Scroll support for long lists
    ScrollArea listScroll_;
};
