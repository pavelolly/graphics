
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
    SetTargetFPS(240);

    SetTraceLogLevel(LOG_DEBUG);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);
    GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(GRAY));

    SceneEllipses         scene_ellipses;
    SceneDrawPolygons     scene_draw_polygons;
    SceneLocalization     scene_localization;
    SceneBezierElementary scene_elementary_bezier;

    Scene *scene = &scene_ellipses;

    while (!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(GetColor(0x181818ff));
            if (scene) {
                scene->Draw();
            } else {
                DrawThePlayground();
            }

        EndDrawing();

        // scene is not switchable when input boxes are active
        if (!scene || scene->IsSwitchable()) {
            switch (GetCharPressed()) {
            case '1':
                ShowCursor(); 
                scene = &scene_ellipses;
                break;
            case '2':
                ShowCursor(); 
                scene = &scene_draw_polygons;
                break;
            case '3':
                HideCursor();
                scene = &scene_localization;
                break;
            case '4':
                ShowCursor(); 
                scene = &scene_elementary_bezier;
                break;
            case '0':
                ShowCursor(); 
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

void DrawThePlayground() {
    DrawLineBezier({100, 100}, {200, 200}, 2, YELLOW);
    GuiGroupBox({300, 300, 400, 400}, "Window"); 

    DrawLineDotted({450, 450}, {500, 500}, 20, 3, GRAY);
    DrawCircleV({450, 450}, 3, RED);
    DrawCircleV({500, 500}, 3, RED);
}