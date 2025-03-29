#include "scene_bezier_elementary.hpp"


std::deque<Point> SceneBezierElementary::GenerateRandomControlPoints(int npoints) {
    if (npoints <= 0) {
        return {};
    }

    std::deque<Point> res;
    int screen_segment_width = static_cast<int>(input_box_panel.panel.x) / npoints;
    for (int i = 0; i < npoints; ++i) {
        res.push_back(GetRandomPoint(screen_segment_width * i, screen_segment_width * (i + 1),
                                     0,                        GetScreenHeight()));
    }
    
    return res;
}

SceneBezierElementary::SceneBezierElementary() :
    input_box_panel(Rectangle{GetScreenWidth() - 400.f, 40, 360, GetScreenHeight() - 80.f})
{
    bezier_curve.control_points = GenerateRandomControlPoints(order + 1);
    bezier_curve.Update();

    dragger.AddToDrag(bezier_curve.control_points);

    input_box_panel.Add(&order, 1, MAX_ORDER, "Order");
    input_box_panel.input_boxes[0].editmode_change_callback =
        [this](bool editmode) {
            if (editmode) {
                return;
            }

            bezier_curve.control_points = GenerateRandomControlPoints(order + 1);
            bezier_curve.Update();

            dragger.Clear();
            dragger.AddToDrag(bezier_curve.control_points);
        };
}

void SceneBezierElementary::Draw() {
    bezier_curve.DrawControlPoints(RED, Fade(GRAY, 0.3f));
    bezier_curve.DrawCurve(YELLOW);

    input_box_panel.Draw();
}

void SceneBezierElementary::Update(float) {
    if (dragger.Update()) {
        bezier_curve.Update();
    }
}

bool SceneBezierElementary::IsSwitchable() {
    return !input_box_panel.input_boxes[0].editmode;
}