#pragma once
// Helper functions for layout calculations in the GUI
//Prevent hardcodding 
#include "raylib.h"
#include <string>

namespace LayoutHelper {
    // Centers a rectangle horizontally and vertically within a given area
    Rectangle CenterRect(float areaX, float areaY, float areaW, float areaH, float rectW, float rectH);

    // Measures and draws text centered horizontally at a given Y position 
    void CenterText(const std::string& text, int fontSize, Color color, float centerX, float y);

    // Measures and draws text, returning its width for further calculations
    float MeasureAndDrawText(const std::string& text, int fontSize, Color color, float x, float y);

    // Creates a grid of rectangles (e.g., for buttons or cells)
    // Returns an array of rectangles; caller MUST delete[]; total count = rows * cols
    Rectangle* CreateGrid(float startX, float startY, int rows, int cols, float cellW, float cellH, float spacing);
};