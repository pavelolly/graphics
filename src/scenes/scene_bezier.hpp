#pragma once

#include <raylib.h>

#include <vector>
#include <deque>

#include <cassert>

#include "geometry/geometry.hpp"
#include "geometry/bezier.hpp"
#include "scenes/point_dragger.hpp"
#include "scenes/scene.hpp"

struct SceneBezier : Scene {
    std::deque<BezierCurve> bezier_curves;
    std::deque<Point> control_points;

    PointDragger dragger;

    void Draw() override {
        for (auto &curve : bezier_curves) {
            curve.DrawControlPoints(RED, Fade(GRAY, 0.3f));
            curve.DrawCurve(YELLOW);
        }

        for (int i = 1; i < static_cast<int>(control_points.size()); ++i) {
            DrawLineDotted(control_points[i - 1], control_points[i], 20, 3, Fade(GRAY, 0.3f));
        }
        for (int i = 0; i < static_cast<int>(control_points.size()); ++i) {
            DrawCircleV(control_points[i], 7, RED);
        }
    };

    size_t GetCurveIdx(size_t idx_point) {
        if (idx_point == 0) {
            return 0;
        }

        return (idx_point - 1) / 3;
    } 

    void Update(float) override {
        if (auto drag_res = dragger.Update(); drag_res.has_value()) {
            size_t idx = drag_res.value();

            size_t first = GetCurveIdx(idx);
            int second = (idx != 0 && idx % 3 == 0) ? static_cast<int>(first + 1) : -1;

            TraceLog(LOG_DEBUG, "Based on idx=%i got first %i and second %i", idx, first, second);

            if (first < bezier_curves.size()) {
                auto begin = control_points.begin() + first * 3;

                assert(std::distance(begin, control_points.end()) >= 4);

                TraceLog(LOG_DEBUG, "Updating curve %i", first);
                bezier_curves[first].SetControlPoints(std::deque<Point>(begin, begin + 4));
            }

            if (second != -1 && second < bezier_curves.size()) {
                auto begin = control_points.begin() + second * 3;

                assert(std::distance(begin, control_points.end()) >= 4);

                TraceLog(LOG_DEBUG, "Updating curve %i", second);
                bezier_curves[second].SetControlPoints(std::deque<Point>(begin, begin + 4));
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            control_points.push_back(GetMousePosition());
            dragger.AddToDrag(control_points.back());

            if (auto size = control_points.size(); size == 4 || (size > 4 && (size - 1) % 3 == 0)) {
                std::deque<Point> tail(control_points.end() - 4, control_points.end());
                assert(tail.size() == 4);

                bezier_curves.push_back(BezierCurve(std::move(tail)));
            }
        }

        if (IsKeyPressed(KEY_DELETE)) {
            bezier_curves.clear();
            control_points.clear();
            dragger.Clear();
        }
    };
};