
#include <raylib.h>

#include "scenes.hpp"

#define WIDTH  1600
#define HEIGHT 900

int main() {
    InitWindow(WIDTH, HEIGHT, "Ellipses");
    SetTargetFPS(60);

    SetTraceLogLevel(LOG_DEBUG);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);

    SceneEllipses     scene_ellipses;
    SceneDrawPolygons scene_draw_polygons;
    SceneLocalization scene_localization;

    Scene *scene = &scene_ellipses;

    while (!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(GetColor(0x181818ff));
            if (scene) {
                scene->Draw();
            } else {
                DrawLineBezier({100, 100}, {200, 200}, 2, YELLOW);
            }

        EndDrawing();

        // scene is not switchable when input boxes are active
        if (!scene || scene->IsSwitchable()) {
            if (IsKeyPressed('1')) {
                EnableCursor();
                scene = &scene_ellipses;
            } else if (IsKeyPressed('2')) {
                EnableCursor();
                scene = &scene_draw_polygons;
            } else if (IsKeyPressed('3')) {
                DisableCursor();
                scene = &scene_localization;

            } else if (IsKeyPressed('0')) {
                EnableCursor();
                scene = nullptr; // empty scene  used for testing
            }
        }
        
        if (scene) {
            scene->Update(GetFrameTime());
        }
    }

    CloseWindow();

    return 0;
}