#include "scene_localization.hpp"


SceneLocalization::SceneLocalization() {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();

    auto GetRandomPoint = [w, h]() {
        return Point { static_cast<float>(GetRandomValue(-w / 2, w / 2)),
                       static_cast<float>(GetRandomValue(-h / 2, h / 2)) };
    };

    triangle.a = GetScreenCenter() + GetRandomPoint();
    triangle.b = GetScreenCenter() + GetRandomPoint();
    triangle.c = GetScreenCenter() + GetRandomPoint();
}

void SceneLocalization::Draw() {
    DrawLineV(triangle.a, triangle.b, YELLOW);
    DrawLineV(triangle.b, triangle.c, YELLOW);
    DrawLineV(triangle.c, triangle.a, YELLOW);

    DrawPoint(triangle.a, RED, 7);
    DrawPoint(triangle.b, RED, 7);
    DrawPoint(triangle.c, RED, 7);

    DrawPoint(GetMousePosition(),
              IsInsideTriangle2(GetMousePosition(), triangle.a, triangle.b, triangle.c) ? GREEN : PURPLE,
              15);
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