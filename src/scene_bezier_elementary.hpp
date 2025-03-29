
#include <vector>

#include "geometry.hpp"
#include "point_dragger.hpp"
#include "scene.hpp"
#include "gui.hpp"


struct SceneBezierElementary : Scene {
    // curve parameters
    std::vector<Point> control_points;
    int order = 3;
    static const int MAX_ORDER = 10;
    int n_control_points = order + 1;

    // panel to cntrol order
    GUI::InputBoxPanel input_box_panel;

    // points of drawn curve
    static const int BEZIER_SEGMENTS = 100;
    std::vector<Point> curve_points;

    PointDragger dragger;

    SceneBezierElementary();

    void UpdateCurve();

    void Draw() override;
    void Update(float) override;
    bool IsSwitchable() override;
};