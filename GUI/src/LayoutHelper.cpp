#include "LayoutHelper.h"

namespace LayoutHelper {
    
    Rectangle CenterRect(float areaX, float areaY, float areaW, float areaH, float rectW, float rectH) {
        float x = areaX + (areaW - rectW) / 2.0f;
        float y = areaY + (areaH - rectH) / 2.0f;
        return {x, y, rectW, rectH};
    }

    void CenterText(const std::string& text, int fontSize, Color color, float centerX, float y) {
        int textWidth = MeasureText(text.c_str(), fontSize);
        float x = centerX - textWidth / 2.0f;
        DrawText(text.c_str(), (int)x, (int)y, fontSize, color);
    }

    float MeasureAndDrawText(const std::string& text, int fontSize, Color color, float x, float y) {
        DrawText(text.c_str(), (int)x, (int)y, fontSize, color);
        return (float)MeasureText(text.c_str(), fontSize);
    }

    // Creates a grid of rectangles 
    Rectangle* CreateGrid(float startX, float startY, int rows, int cols, float cellW, float cellH, float spacing) {
        int totalRects = rows * cols;
        Rectangle* rects = new Rectangle[totalRects];
        int index = 0;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                float x = startX + c * (cellW + spacing);
                float y = startY + r * (cellH + spacing);
                rects[index] = {x, y, cellW, cellH};
                index++;
            }
        }
        return rects;
    }
}