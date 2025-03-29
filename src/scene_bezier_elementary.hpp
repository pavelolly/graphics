
#include <vector>

#include "geometry.hpp"
#include "bezier.hpp"
#include "point_dragger.hpp"
#include "scene.hpp"
#include "gui.hpp"


struct SceneBezierElementary : Scene {
    // curve parameters
    BezierCurve bezier_curve;

    int order = 3;  // order variable that is modified by input_box

    static const int MAX_ORDER = 10;

    // panel to cntrol order
    GUI::InputBoxPanel input_box_panel;

    PointDragger dragger;

    SceneBezierElementary();

    std::deque<Point> GenerateRandomControlPoints(int npoints);

    void Draw() override;
    void Update(float) override;
    bool IsSwitchable() override;
};