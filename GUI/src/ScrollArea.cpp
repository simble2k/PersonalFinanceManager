  
#include "ScrollArea.h"

// Helper functions
static inline float maxf(float a, float b) { return (a > b) ? a : b; }
static inline float minf(float a, float b) { return (a < b) ? a : b; }

void ScrollArea::Init(float x, float y, float w, float h) {
    view = { x, y, w, h };
    offset = 0.0f;
    contentHeight = 0.0f;
    dragging = false;
    thumb = { 0,0,12,0 };
    grabY = 0.0f;
}

static float clampf(float v, float a, float b) { return (v < a) ? a : ((v > b) ? b : v); } // clamp float to [a,b]

void ScrollArea::Update(float newContentHeight) {
    contentHeight = newContentHeight;

    // Clamp offset to valid range
    if (contentHeight <= view.height) {
        offset = 0.0f;
    } else {
        offset = clampf(offset, 0.0f, contentHeight - view.height);
    }

    // Wheel input (works when mouse is over area)
    Vector2 m = GetMousePosition();
    if (CheckCollisionPointRec(m, view)) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            offset -= wheel * wheelSpeed;
            offset = clampf(offset, 0.0f, maxf(0.0f, contentHeight - view.height));
        }
    }

    // Compute scrollbar thumb when needed  
    if (contentHeight > view.height) {
        float trackX = view.x + view.width - 12.0f;
        float trackW = 12.0f;
        float viewH = view.height;
        float thumbH = (viewH * viewH) / contentHeight; // proportional size
        thumbH = maxf(minThumbHeight, thumbH);
        float avail = viewH - thumbH;
        float tY = view.y + (offset / (contentHeight - viewH)) * avail;
        thumb = { trackX, tY, trackW, thumbH };
    } else {
        thumb = { 0,0,0,0 };
    }

    // Dragging logic
    Vector2 mouse = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    if (mousePressed) {
        if (CheckCollisionPointRec(mouse, thumb)) {
            dragging = true;
            grabY = mouse.y - thumb.y;
        } else if (CheckCollisionPointRec(mouse, Rectangle{ view.x + view.width - 12.0f, view.y, 12.0f, view.height })) {
            // Click on track -> page up/down
            if (mouse.y < thumb.y) {
                // page up
                offset -= view.height * 0.9f;
            } else if (mouse.y > (thumb.y + thumb.height)) {
                // page down
                offset += view.height * 0.9f;
            }
            offset = clampf(offset, 0.0f, maxf(0.0f, contentHeight - view.height));
        }
    }

    if (dragging) {
        if (mouseDown) {
            // move thumb with mouse
            float thumbH = thumb.height;
            float minY = view.y;
            float maxY = view.y + view.height - thumbH;
            float newThumbY = clampf(mouse.y - grabY, minY, maxY);
            float avail = view.height - thumbH;
            float frac = (avail <= 0.0f) ? 0.0f : ((newThumbY - view.y) / avail);
            offset = frac * (contentHeight - view.height);
        } else {
            // mouse released
            dragging = false;
        }
    }
}

void ScrollArea::Begin() {
    // Enable scissor to clip drawing to view rectangle
    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);
}

void ScrollArea::End() {
    EndScissorMode();

    // Draw scrollbar if needed
    if (contentHeight > view.height) {
        Rectangle track = { view.x + view.width - 12.0f, view.y, 12.0f, view.height };
        DrawRectangleRec(track, Fade(GRAY, 0.25f));
        DrawRectangleRec(thumb, Fade(DARKGRAY, 0.9f));

        // optional: draw thumb border
        DrawRectangleLinesEx(thumb, 1.0f, DARKBLUE);
    }
}