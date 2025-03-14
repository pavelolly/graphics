
#include <raylib.h>

#include "scene_ellipses.hpp"
#include "scene_draw_polygons.hpp"
#include "scene_localization.hpp"
#include "scene_elementary_bezier.hpp"

#define WIDTH  1600
#define HEIGHT 900

int main() {
    InitWindow(WIDTH, HEIGHT, "Graphics");
    SetTargetFPS(240);

    SetTraceLogLevel(LOG_DEBUG);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);
    GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(GRAY));

    SceneEllipses         scene_ellipses;
    SceneDrawPolygons     scene_draw_polygons;
    SceneLocalization     scene_localization;
    SceneElementaryBezier scene_elementary_bezier;

    Scene *scene = &scene_ellipses;

    while (!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(GetColor(0x181818ff));
            if (scene) {
                scene->Draw();
            } else {

                DrawLineBezier({100, 100}, {200, 200}, 2, YELLOW);
                GuiGroupBox({300, 300, 400, 400}, "Window"); 
                
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
            } else if (IsKeyPressed('4')) {
                EnableCursor();
                scene = &scene_elementary_bezier;
            } else if (IsKeyPressed('0')) {
                EnableCursor();
                scene = nullptr; // empty scene used for testing
            }
        }
        
        if (scene) {
            scene->Update(GetFrameTime());
        }
    }

    CloseWindow();

    return 0;
}