#include "scene_localization.hpp"

#include <raylib.h>
#include <raygui.h>

#include <cassert>
#include <string>
#include <cmath>

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
    Color col_side1 = YELLOW;
    Color col_side2 = YELLOW;
    Color col_side3 = YELLOW;

    Point mouse_pos = GetMousePosition();

    Point &a = triangle.a;
    Point &b = triangle.b;
    Point &c = triangle.c;
    Point &p = mouse_pos;

    switch (mode) {
        case 0: // barycentric
        {
            Point &a = triangle.a;

            float d  = (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
            float k1 = ((b.x - p.x) * (c.y - p.y) - (c.x - p.x) * (b.y - p.y)) / d;
            float k2 = ((p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y)) / d;
            float k3 = 1 - k1 - k2;

            std::string text = "k1 = " + std::to_string(k1);
            DrawText(text.c_str(), 40, 40, 20, k1 > 0 ? GRAY : Fade(GRAY, 0.3f));

            text = "k2 = " + std::to_string(k2);
            DrawText(text.c_str(), 40, 60, 20, k2 > 0 ? GRAY : Fade(GRAY, 0.3f));
            
            text = "k3 = " + std::to_string(k3);
            DrawText(text.c_str(), 40, 80, 20, k3 > 0 ? GRAY : Fade(GRAY, 0.3f));
            break;
        }
        case 1: // sides
        {
            bool side1 = std::signbit((p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x));
            bool side2 = std::signbit((p.x - b.x) * (c.y - b.y) - (p.y - b.y) * (c.x - b.x));
            bool side3 = std::signbit((p.x - c.x) * (a.y - c.y) - (p.y - c.y) * (a.x - c.x));

            if (!side1) {
                col_side1 = ORANGE;
            }
            if (!side2) {
                col_side2 = ORANGE;
            }
            if (!side3) {
                col_side3 = ORANGE;
            }

            DrawText("1", static_cast<int>(a.x) - 20, static_cast<int>(a.y) - 20, GuiGetStyle(DEFAULT, TEXT_SIZE), GRAY);
            DrawText("2", static_cast<int>(b.x) - 20, static_cast<int>(b.y) - 20, GuiGetStyle(DEFAULT, TEXT_SIZE), GRAY);
            DrawText("3", static_cast<int>(c.x) - 20, static_cast<int>(c.y) - 20, GuiGetStyle(DEFAULT, TEXT_SIZE), GRAY);

            break;
        }
        case 2: // rays
        {
            Point center = (a + b + c) / 3;

            DrawLineDotted(p, center, 20, 5, Fade(GRAY, 0.3f));
            DrawCircleV(center, 7, RED);

            if (auto i = Intersect(p, center, a, b)) {
                col_side1 = ORANGE;
                DrawCircleV(i.value(), 7, PURPLE);
            }

            if (auto i = Intersect(p, center, b, c)) {
                col_side2 = ORANGE;
                DrawCircleV(i.value(), 7, PURPLE);
            }

            if (auto i = Intersect(p, center, c, a)) {
                col_side3 = ORANGE;
                DrawCircleV(i.value(), 7, PURPLE);
            }

            break;
        }
        default:
            assert(false && "unreachable");
    }

    DrawLineV(triangle.a, triangle.b, col_side1);
    DrawLineV(triangle.b, triangle.c, col_side2);
    DrawLineV(triangle.c, triangle.a, col_side3);

    DrawCircleV(triangle.a, 7, RED);
    DrawCircleV(triangle.b, 7, RED);
    DrawCircleV(triangle.c, 7, RED);

    DrawCircleV(GetMousePosition(), 15, is_inside_funcs[mode](mouse_pos, triangle.a, triangle.b, triangle.c) ? GREEN : PURPLE);
}

void SceneLocalization::Update(float) {
    dragger.Update();

    if (IsKeyPressed(KEY_SPACE)) {
        mode = (mode + 1) % is_inside_funcs.size();
    }
}