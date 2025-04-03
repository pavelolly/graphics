#include "scene_bezier.hpp"

#include "colors.h"

#include <raygui.h>

void SceneBezier::Draw() {
    BeginMode2D(camera);

    for (auto &set : bezier_sets) {

        Color color_point = COLOR_POINT_PRIMARY;
        Color color_curve = COLOR_LINE_PRIMARY;
        if (&set == &bezier_sets.back() && !need_new_set) {
            color_point = COLOR_POINT_SECONDARY;
            color_curve = COLOR_LINE_SECONDARY;
        }

        for (auto &curve : set.curves) {
            if (show_control_points) {
                curve.DrawControlPoints(color_point, COLOR_GRAY_FADED);
            }
            curve.DrawCurve(color_curve);
        }
    }
    

    if (show_control_points) {
        for (auto &set : bezier_sets) {
            
            Color color_point = &set == &bezier_sets.back() && !need_new_set ? COLOR_POINT_SECONDARY : COLOR_POINT_PRIMARY;

            for (int i = 1; (size_t) i < set.control_points.size(); ++i) {
                DrawLineDotted(set.control_points[i - 1], set.control_points[i], 20, 3, COLOR_GRAY_FADED);
            }
            for (int i = 0; (size_t) i < set.control_points.size(); ++i) {
                DrawCircleV(set.control_points[i], 7, color_point);
            }
        }
    }

    EndMode2D();

    DrawText("Bezier Curves", 20, 20, GuiGetStyle(DEFAULT, TEXT_SIZE), GRAY);
};

void SceneBezier::Update(float dt) {
    if (show_control_points) {

        if (auto drag_res = dragger.Update(); drag_res.has_value()) {
            size_t global_idx = drag_res.value();

            int idx = (int) global_idx;
            int set_idx = -1;
            for (int i = 0; (size_t) i < bezier_sets.size(); ++i) {
                int n_control_points = (int) bezier_sets[i].control_points.size();
                if (idx < n_control_points) {
                    set_idx = i;
                    break;
                }

                idx -= n_control_points;
            }

            TraceLog(LOG_DEBUG, "Global point idx=%i; set_idx=%i, idx=%i", global_idx, set_idx, idx);

            assert(set_idx != -1);

            // we generally want to update two curves because they share some points
            size_t first = idx == 0
                             ? 0
                             : (idx - 1) / BEZIER_ORDER;

            int second = (idx != 0 && idx % BEZIER_ORDER == 0)
                             ? (int) (first + 1)
                             : -1;

            auto &[curves, control_points] = bezier_sets[set_idx];

            if (first < curves.size()) {
                auto begin = control_points.begin() + first * BEZIER_ORDER;
                auto end   = control_points.end();

                assert((size_t) std::distance(begin, end) >= ELEM_CONTROL_POINTS);

                TraceLog(LOG_DEBUG, "Based on idx=%i got first %i and second %i", idx, first, second);
                TraceLog(LOG_DEBUG, "Updating curve %i", first);
                curves[first].SetControlPoints(std::deque<Point>(begin, begin + ELEM_CONTROL_POINTS));
            }

            if (second != -1 && (size_t) second < curves.size()) {
                auto begin = control_points.begin() + second * BEZIER_ORDER;
                auto end   = control_points.end();

                assert((size_t) std::distance(begin, end) >= ELEM_CONTROL_POINTS);

                TraceLog(LOG_DEBUG, "Updating curve %i", second);
                curves[second].SetControlPoints(std::deque<Point>(begin, begin + ELEM_CONTROL_POINTS));
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (need_new_set) {
                bezier_sets.emplace_back();
                need_new_set = false;
            }

            auto &[curves, control_points] = bezier_sets.back();

            Point new_point = GetScreenToWorld2D(GetMousePosition(), camera);

            control_points.push_back(new_point);
            dragger.AddToDrag(control_points.back());

            if (size_t size = control_points.size(); size == ELEM_CONTROL_POINTS || (size > ELEM_CONTROL_POINTS && (size - 1) % BEZIER_ORDER == 0)) {
                std::deque<Point> tail(control_points.end() - ELEM_CONTROL_POINTS, control_points.end());
                assert(tail.size() == ELEM_CONTROL_POINTS);

                curves.push_back(BezierCurve(std::move(tail)));
            }
        }

    }

    if (float wheel_move = GetMouseWheelMove(); wheel_move != 0.f) {
        float scale = 1.f + 0.1f * std::abs(wheel_move);
        if (wheel_move < 0) {
            scale = 1.f / scale;
        }
        camera.zoom = Clamp(camera.zoom * scale, 0.125f, 64.f);

        Point mouse_pos = GetMousePosition();

        camera.target = GetScreenToWorld2D(mouse_pos, camera);
        camera.offset = mouse_pos;
    }

    Point shift = Vector2Zeros;
    if (IsKeyDown(KEY_LEFT)) {
        shift += {-1, 0};
    }
    if (IsKeyDown(KEY_RIGHT)) {
        shift += {1, 0};
    }
    if (IsKeyDown(KEY_UP)) {
        shift += {0, -1};
    }
    if (IsKeyDown(KEY_DOWN)) {
        shift += {0, 1};
    }
    shift = Vector2Normalize(shift) * 200 * dt;

    camera.offset += shift;

    if (IsKeyPressed(KEY_SPACE)) {
        show_control_points = !show_control_points;
    }

    if (IsKeyPressed('L') && bezier_sets.size() > 0) {
        auto &control_points = bezier_sets.back().control_points;
        for (size_t i = BEZIER_ORDER; i + 1 < control_points.size(); i += BEZIER_ORDER) {
            control_points[i] = Project(control_points[i], control_points[i - 1], control_points[i + 1]);
            UpdateAllCurves();
        }
    }

    if (IsKeyPressed(KEY_DELETE)) {
        bezier_sets.clear();
        dragger.Clear();
    }

    if (IsKeyPressed(KEY_ENTER)) {
        need_new_set = true;
        // TODO: strip off unused control points
    }
};

void SceneBezier::UpdateAllCurves() {
    for (auto &[curves, control_points] : bezier_sets) {
        for (int curve_idx = 0, curve_first_control_point = 0;
             (size_t) curve_idx < curves.size();
             curve_first_control_point += BEZIER_ORDER, ++curve_idx)
        {
            std::deque<Point> chunk(control_points.begin() + curve_first_control_point,
                                    control_points.begin() + curve_first_control_point + ELEM_CONTROL_POINTS);

            curves[curve_idx].SetControlPoints(std::move(chunk));
        }
    }
}