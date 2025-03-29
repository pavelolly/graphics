#include "scene_localization.hpp"


SceneLocalization::SceneLocalization() {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();

    triangle.a = GetScreenCenter() + GetRandomPoint(-w / 2, w / 2, -h / 2, h / 2);
    triangle.b = GetScreenCenter() + GetRandomPoint(-w / 2, w / 2, -h / 2, h / 2);
    triangle.c = GetScreenCenter() + GetRandomPoint(-w / 2, w / 2, -h / 2, h / 2);

    dragger.AddToDrag(triangle.a);
    dragger.AddToDrag(triangle.b);
    dragger.AddToDrag(triangle.c);
}

void SceneLocalization::Draw() {
    DrawLineV(triangle.a, triangle.b, YELLOW);
    DrawLineV(triangle.b, triangle.c, YELLOW);
    DrawLineV(triangle.c, triangle.a, YELLOW);

    DrawCircleV(triangle.a, 7, RED);
    DrawCircleV(triangle.b, 7, RED);
    DrawCircleV(triangle.c, 7, RED);

    DrawCircleV(GetMousePosition(), 15,
                IsInsideTriangle2(GetMousePosition(), triangle.a, triangle.b, triangle.c) ? GREEN : PURPLE);
}

void SceneLocalization::Update(float) {
    dragger.Update();
}