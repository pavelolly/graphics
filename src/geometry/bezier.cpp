#include <raylib.h>
#include <source_location>

#include "bezier.hpp"

BezierCurve::BezierCurve(int bezier_segments) : bezier_segments(bezier_segments) {
    curve_points.reserve(bezier_segments + 1);
}

void BezierCurve::DrawControlPoints(Color color_points, Color color_lines) const {
    for (int i = 1; i < static_cast<int>(control_points.size()); ++i) {
        DrawLineDotted(control_points[i - 1], control_points[i], 20, 3, color_lines);
    }
    for (int i = 0; i < static_cast<int>(control_points.size()); ++i) {
        DrawCircleV(control_points[i], 7, color_points);
    }
}

void BezierCurve::DrawCurve(Color color) const {
    for (int i = 1; i < static_cast<int>(curve_points.size()); ++i) {
        DrawLineV(curve_points[i - 1], curve_points[i], color);
    }
}

void BezierCurve::Update() {
    // calculate bezier curve
    auto bezier_func = BezierFunc(control_points);
    if (!bezier_func) {
        TraceLog(LOG_WARNING, "%s: Failed to compure bezier_func", std::source_location::current().function_name());
        return;
    }

    curve_points.clear();
    for (int i = 0; i <= bezier_segments; ++i) {
        float t = static_cast<float>(i) / bezier_segments;
        curve_points.push_back(bezier_func(t));
    }
}