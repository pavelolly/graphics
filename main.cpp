
#include <raylib.h>

#include <deque>

#include "geometry.h"
#include "gui.h"
#include "scenes.h"

#include "log_macros.h"

#define WIDTH  1600
#define HEIGHT 900

int main() {
    InitWindow(WIDTH, HEIGHT, "Ellipses");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_DEBUG);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);

    SceneEllipses scene_ellipses;
    SceneDrawPolygons scene_draw_polygons;

    Scene *scene = &scene_ellipses;

    while (!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(GetColor(0x181818ff));
            scene->Draw();

        EndDrawing();

        if (IsKeyPressed('Q')) {
            scene = &scene_ellipses;
        }

        if (IsKeyPressed('E')) {
            scene = &scene_draw_polygons;
        }

        scene->Update(GetFrameTime());
    }

    CloseWindow();

    return 0;
}