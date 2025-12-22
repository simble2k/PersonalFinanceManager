#include <iostream>
#include "raylib.h"
#include "App.h"

// This file is the GUI entrypoint. It stays minimal: initialize raylib, create the
// App instance, and run the main loop. Heavy UI logic is delegated to App / myprevwork code.

int main() {
    const int WIDTH = 1280;
    const int HEIGHT = 800;

    InitWindow(WIDTH, HEIGHT, "Personal Finance Manager - GUI");
    SetTargetFPS(60);

    App app;
    if (!app.Init()) {
        // Initialization failed (e.g. resource loading). Exit.
        CloseWindow();
        return -1;
    }

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        app.Update(dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        app.Draw();
        EndDrawing();
    }

    app.Shutdown();
    CloseWindow();
    return 0;
}

