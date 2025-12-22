
#pragma once
#include "raylib.h"

using namespace std;

// Simple vertical scroll area helper for raylib.
// - Initialize with a view rect via Init()
// - Call Update(contentHeight) each frame (before drawing) to handle input.
// - Call Begin() before drawing content and End() after to clip and draw the scrollbar.
// - GetOffset() returns the vertical offset (pixels) to apply to content rendering.
struct ScrollArea {
    Rectangle view = {0,0,0,0};
    float offset = 0.0f;            // current scroll offset (0 = top)
    float contentHeight = 0.0f;     // height of full content (pixels)
    bool dragging = false;
    Rectangle thumb = {0,0,0,0};
    float grabY = 0.0f;
    float wheelSpeed = 40.0f;       // pixels per wheel tick
    float minThumbHeight = 24.0f;

    void Init(float x, float y, float w, float h);
    void Update(float newContentHeight);
    void Begin(); // Begin scissor/clipping region
    void End();   // End scissor and draw scrollbar
    float GetOffset() const { return offset; }
};