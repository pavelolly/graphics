#include "scene_bezier_elementary.hpp"

SceneBezierElementary::SceneBezierElementary() :
    input_box_panel(Rectangle{GetScreenWidth() - 400.f, 40, 360, GetScreenHeight() - 80.f})
{
    control_points.reserve(MAX_ORDER + 1);
    curve_points.reserve(BEZIER_SEGMENTS + 1);
    UpdateCurve();

    input_box_panel.Add(&order, 1, MAX_ORDER, "Order");
    input_box_panel.input_boxes[0].editmode_change_callback =
        [this](bool editmode) {
            if (editmode) {
                return;
            }

            n_control_points = order + 1;
            UpdateCurve();
        };
}

void SceneBezierElementary::UpdateCurve() {
    // generate control points if needed
    if (n_control_points != control_points.size()) {
        control_points.clear();
        int screen_segment_width = static_cast<int>(input_box_panel.panel.x) / n_control_points;
        for (int i = 0; i < n_control_points; ++i) {
            control_points.push_back(GetRandomPoint(screen_segment_width * i, screen_segment_width * (i + 1), 0, GetScreenHeight()));
        }
    }

    // calculate bezier curve
    auto bezier_func = BezierFunc(control_points);
    assert(bezier_func && "Failed to get Bezier func");

    curve_points.clear();
    for (int i = 0; i <= BEZIER_SEGMENTS; ++i) {
        float t = static_cast<float>(i) / BEZIER_SEGMENTS;
        curve_points.push_back(bezier_func(t));
    }
}

void SceneBezierElementary::Draw() {
    // draw control points
    for (int i = 1; i < n_control_points; ++i) {
        DrawLineDotted(control_points[i - 1], control_points[i], 20, 3, Fade(GRAY, 0.3f));
    }
    for (int i = 0; i < n_control_points; ++i) {
        DrawCircleV(control_points[i], 7, RED);
    }

    // draw bezier curve
    for (int i = 1; i < curve_points.size(); ++i) {
        DrawLineV(curve_points[i - 1], curve_points[i], YELLOW);
    }

    input_box_panel.Draw();
}

void SceneBezierElementary::Update(float) {
    if (dragging) {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            assert(dragged_point && "dragged_point is nullptr when trying to drag");

            Point mouse_delta = GetMouseDelta();
            if (mouse_delta != Vector2Zeros) {
                *dragged_point += GetMouseDelta();
                UpdateCurve();
            }
        }

        if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT)) {
            dragging = false;
            dragged_point = nullptr;
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Point mouse_pos = GetMousePosition();

        for (auto &point : control_points) {
            if (CheckCollisionPointCircle(mouse_pos, point, 10)) {
                dragging = true;
                dragged_point = &point;
                break;
            }
        }
    }
}

bool SceneBezierElementary::IsSwitchable() {
    return !input_box_panel.input_boxes[0].editmode;
}