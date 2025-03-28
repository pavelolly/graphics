
#include <raylib.h>

#include "scene_ellipses.hpp"
#include "scene_draw_polygons.hpp"
#include "scene_localization.hpp"
#include "scene_bezier_elementary.hpp"

#define WIDTH  1600
#define HEIGHT 900

void DrawThePlayground();

int main() {
    InitWindow(WIDTH, HEIGHT, "Graphics");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    SetTraceLogLevel(LOG_DEBUG);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);
    GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(GRAY));

    SceneDrawPolygons     scene_draw_polygons;
    SceneLocalization     scene_localization;
    SceneBezierElementary scene_elementary_bezier;
    SceneEllipses         scene_ellipses;

    Scene *scene = &scene_draw_polygons;

    while (!WindowShouldClose()) {

        // scene is not switchable when input boxes are active
        if (!scene || scene->IsSwitchable()) {
            switch (GetCharPressed()) {
            case '1':
                ShowCursor(); 
                scene = &scene_draw_polygons;
                break;
            case '2':
                HideCursor();
                scene = &scene_localization;
                break;
            case '3':
                ShowCursor(); 
                scene = &scene_elementary_bezier;
                break;
            case '4':
                ShowCursor(); 
                scene = &scene_ellipses;
                break;
            case '0':
                ShowCursor(); 
                scene = nullptr; // empty scene used for testing
            }
        }
        
        if (scene) {
            scene->Update(GetFrameTime());
        }

        BeginDrawing();

            ClearBackground(GetColor(0x181818ff));
            if (scene) {
                scene->Draw();
            } else {
                DrawThePlayground();
            }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void DrawThePlayground() {
    DrawLineBezier({100, 100}, {200, 200}, 2, YELLOW);
    GuiGroupBox({300, 300, 400, 400}, "Window"); 

    // DrawCircleV({450, 450}, 3, RED);
    // DrawCircleV({500, 500}, 3, RED);
    DrawLineDotted({450, 450}, {500, 500}, 20, 5, GRAY);

    DrawLineEx({600, 300}, {601, 300}, 16, GRAY);
}