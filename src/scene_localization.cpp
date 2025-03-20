#include "scene_localization.hpp"


SceneLocalization::SceneLocalization() {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();

    triangle.a = GetScreenCenter() + GetRandomPoint(-w / 2, w / 2, -h / 2, h / 2);
    triangle.b = GetScreenCenter() + GetRandomPoint(-w / 2, w / 2, -h / 2, h / 2);
    triangle.c = GetScreenCenter() + GetRandomPoint(-w / 2, w / 2, -h / 2, h / 2);
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
    if (dragging) {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            assert(dragged_point && "dragged_point is nullptr when trying to drag");

            *dragged_point += GetMouseDelta();
        }

        if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT)) {
            dragging = false;
            dragged_point = nullptr;
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Point mouse_pos = GetMousePosition();

        if (CheckCollisionPointCircle(mouse_pos, triangle.a, 10)) {
            dragging = true;
            dragged_point = &triangle.a;
        }
        if (CheckCollisionPointCircle(mouse_pos, triangle.b, 10)) {
            dragging = true;
            dragged_point = &triangle.b;
        }
        if (CheckCollisionPointCircle(mouse_pos, triangle.c, 10)) {
            dragging = true;
            dragged_point = &triangle.c;
        }
    }
}