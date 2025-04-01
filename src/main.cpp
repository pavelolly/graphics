
#include <raylib.h>

#include "scenes/scene_ellipses.hpp"
#include "scenes/scene_draw_polygons.hpp"
#include "scenes/scene_localization.hpp"
#include "scenes/scene_bezier_elementary.hpp"
#include "scenes/scene_bezier.hpp"

#include "colors.h"

#define WIDTH  1600
#define HEIGHT 900

#define TARGET_FPS (GetMonitorRefreshRate(GetCurrentMonitor()))

void DrawThePlayground();

int main() {
    InitWindow(WIDTH, HEIGHT, "Graphics");
    SetTargetFPS(TARGET_FPS);

    SetTraceLogLevel(LOG_DEBUG);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(GRAY));

    SceneDrawPolygons     scene_draw_polygons;
    SceneLocalization     scene_localization;
    SceneBezierElementary scene_elementary_bezier;
    SceneBezier           scene_bezier;
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
                ShowCursor(); 
                scene = &scene_ellipses;
                break;
            case '3':
                HideCursor();
                scene = &scene_localization;
                break;
            case '4':
                ShowCursor(); 
                scene = &scene_elementary_bezier;
                break;
            case '5':
                ShowCursor();
                scene = &scene_bezier;
                break;

            case '0':
                ShowCursor(); 
                scene = nullptr; // empty scene used for testing
                break;
            }
        }
        
        if (scene) {
            scene->Update(GetFrameTime());
        }

        BeginDrawing();

            ClearBackground(COLOR_BACKGROUND);
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
    DrawLineBezier({100, 100}, {200, 200}, 2, COLOR_LINE_PRIMARY);
    GuiGroupBox({300, 300, 400, 400}, "Window"); 

    // DrawCircleV({450, 450}, 3, COLOR_POINT_PRIMARY);
    // DrawCircleV({500, 500}, 3, COLOR_POINT_PRIMARY);
    DrawLineDotted({450, 450}, {500, 500}, 20, 5, GRAY);

    DrawLineEx({600, 300}, {601, 300}, 16, GRAY);

    Point line1[] = { { 900, 560 }, { 1200, 800 } };
    Point line2[] = { { 900, 700 }, { 1300, 670 } };

    DrawLineEx(line1[0], line1[1], 5, COLOR_LINE_SECONDARY);
    DrawLineEx(line2[0], line2[1], 5, COLOR_LINE_SECONDARY);

    if (auto i = Intersect(line1[0], line1[1], line2[0], line2[1])) {
        DrawCircleV({ 100, 100 }, 7, BROWN);
        DrawCircleV(i.value(), 7, COLOR_POINT_SECONDARY);
    }
}