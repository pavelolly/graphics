#include "scene_bezier.hpp"

void SceneBezier::Draw() {
    for (auto &curve : bezier_curves) {
        if (show_control_points) {
            curve.DrawControlPoints(RED, Fade(GRAY, 0.3f));
        }
        curve.DrawCurve(YELLOW);
    }

    if (show_control_points) {
        for (int i = 1; i < static_cast<int>(control_points.size()); ++i) {
            DrawLineDotted(control_points[i - 1], control_points[i], 20, 3, Fade(GRAY, 0.3f));
        }
        for (int i = 0; i < static_cast<int>(control_points.size()); ++i) {
            DrawCircleV(control_points[i], 7, RED);
        }
    }
};

void SceneBezier::Update(float) {
    if (show_control_points) {

        if (auto drag_res = dragger.Update(); drag_res.has_value()) {
            size_t idx = drag_res.value();

            // we generally want to update two curves because they share some points
            size_t first = idx == 0
                             ? 0
                             : (idx - 1) / BEZIER_ORDER;

            int second = (idx != 0 && idx % BEZIER_ORDER == 0)
                             ? static_cast<int>(first + 1)
                             : -1;

            if (first < bezier_curves.size()) {
                auto begin = control_points.begin() + first * BEZIER_ORDER;

                assert(std::distance(begin, control_points.end()) >= ELEM_CONTROL_POINTS);

                TraceLog(LOG_DEBUG, "Based on idx=%i got first %i and second %i", idx, first, second);
                TraceLog(LOG_DEBUG, "Updating curve %i", first);
                bezier_curves[first].SetControlPoints(std::deque<Point>(begin, begin + ELEM_CONTROL_POINTS));
            }

            if (second != -1 && second < bezier_curves.size()) {
                auto begin = control_points.begin() + second * BEZIER_ORDER;

                assert(std::distance(begin, control_points.end()) >= ELEM_CONTROL_POINTS);

                TraceLog(LOG_DEBUG, "Updating curve %i", second);
                bezier_curves[second].SetControlPoints(std::deque<Point>(begin, begin + ELEM_CONTROL_POINTS));
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            control_points.push_back(GetMousePosition());
            dragger.AddToDrag(control_points.back());

            if (size_t size = control_points.size(); size == ELEM_CONTROL_POINTS || size > ELEM_CONTROL_POINTS && (size - 1) % BEZIER_ORDER == 0) {
                std::deque<Point> tail(control_points.end() - ELEM_CONTROL_POINTS,
                                       control_points.end());
                assert(tail.size() == ELEM_CONTROL_POINTS);

                bezier_curves.push_back(BezierCurve(std::move(tail)));
            }
        }

    }

    if (IsKeyPressed(KEY_SPACE)) {
        show_control_points = !show_control_points;
    }

    if (IsKeyPressed('L')) {
        for (size_t i = BEZIER_ORDER; i + 1 < control_points.size(); i += BEZIER_ORDER) {
            control_points[i] = Project(control_points[i], control_points[i - 1], control_points[i + 1]);
            UpdateAllCurves();
        }
    }

    if (IsKeyPressed(KEY_DELETE)) {
        bezier_curves.clear();
        control_points.clear();
        dragger.Clear();
    }
};

void SceneBezier::UpdateAllCurves() {
    for (int curve_idx = 0, curve_first_control_point = 0;
         curve_idx < bezier_curves.size();
         curve_first_control_point += BEZIER_ORDER, ++curve_idx)
    {
        std::deque<Point> chunk(control_points.begin() + curve_first_control_point,
                                control_points.begin() + curve_first_control_point + ELEM_CONTROL_POINTS);
        
        bezier_curves[curve_idx].SetControlPoints(std::move(chunk));
    }
}