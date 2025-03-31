#include "scene_bezier.hpp"

void SceneBezier::Draw() {
    for (auto &set : bezier_sets) {
        for (auto &curve : set.curves) {
            if (show_control_points) {
                curve.DrawControlPoints(RED, Fade(GRAY, 0.3f));
            }
            curve.DrawCurve(YELLOW);
        }
    }
    

    if (show_control_points) {
        for (auto &set : bezier_sets) {
            for (int i = 1; i < static_cast<int>(set.control_points.size()); ++i) {
                DrawLineDotted(set.control_points[i - 1], set.control_points[i], 20, 3, Fade(GRAY, 0.3f));
            }
            for (int i = 0; i < static_cast<int>(set.control_points.size()); ++i) {
                DrawCircleV(set.control_points[i], 7, RED);
            }
        }
    }
};

void SceneBezier::Update(float) {
    if (show_control_points) {

        if (auto drag_res = dragger.Update(); drag_res.has_value()) {
            int global_idx = static_cast<int>(drag_res.value());

            int idx = global_idx;
            int set_idx = -1;
            for (int i = 0; i < bezier_sets.size(); ++i) {
                if (idx < bezier_sets[i].control_points.size()) {
                    set_idx = i;
                    break;
                }

                idx -= static_cast<int>(bezier_sets[i].control_points.size());
            }

            TraceLog(LOG_DEBUG, "Global point idx=%i; set_idx=%i, idx=%i", global_idx, set_idx, idx);

            assert(set_idx != -1);

            // we generally want to update two curves because they share some points
            size_t first = idx == 0
                             ? 0
                             : (idx - 1) / BEZIER_ORDER;

            int second = (idx != 0 && idx % BEZIER_ORDER == 0)
                             ? static_cast<int>(first + 1)
                             : -1;

            if (first < bezier_sets[set_idx].curves.size()) {
                auto begin = bezier_sets[set_idx].control_points.begin() + first * BEZIER_ORDER;
                auto end   = bezier_sets[set_idx].control_points.end();

                assert(std::distance(begin, end) >= ELEM_CONTROL_POINTS);

                TraceLog(LOG_DEBUG, "Based on idx=%i got first %i and second %i", idx, first, second);
                TraceLog(LOG_DEBUG, "Updating curve %i", first);
                bezier_sets[set_idx].curves[first].SetControlPoints(std::deque<Point>(begin, begin + ELEM_CONTROL_POINTS));
            }

            if (second != -1 && second < bezier_sets[set_idx].curves.size()) {
                auto begin = bezier_sets[set_idx].control_points.begin() + second * BEZIER_ORDER;
                auto end   = bezier_sets[set_idx].control_points.end();

                assert(std::distance(begin, end) >= ELEM_CONTROL_POINTS);

                TraceLog(LOG_DEBUG, "Updating curve %i", second);
                bezier_sets[set_idx].curves[second].SetControlPoints(std::deque<Point>(begin, begin + ELEM_CONTROL_POINTS));
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (need_new_set) {
                bezier_sets.emplace_back();
                need_new_set = false;
            }

            auto &control_points = bezier_sets.back().control_points;
            auto &curves         = bezier_sets.back().curves;
            control_points.push_back(GetMousePosition());
            dragger.AddToDrag(control_points.back());

            if (size_t size = control_points.size(); size == ELEM_CONTROL_POINTS || size > ELEM_CONTROL_POINTS && (size - 1) % BEZIER_ORDER == 0) {
                std::deque<Point> tail(control_points.end() - ELEM_CONTROL_POINTS,
                                       control_points.end());
                assert(tail.size() == ELEM_CONTROL_POINTS);

                curves.push_back(BezierCurve(std::move(tail)));
            }
        }

    }

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
    }
};

void SceneBezier::UpdateAllCurves() {
    for (auto &[curves, control_points] : bezier_sets) {
        for (int curve_idx = 0, curve_first_control_point = 0;
             curve_idx < curves.size();
             curve_first_control_point += BEZIER_ORDER, ++curve_idx)
        {
            std::deque<Point> chunk(control_points.begin() + curve_first_control_point,
                                    control_points.begin() + curve_first_control_point + ELEM_CONTROL_POINTS);

            curves[curve_idx].SetControlPoints(std::move(chunk));
        }
    }
}