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

    static constexpr size_t BEZIER_ORDER = 1;
    static constexpr size_t ELEM_CONTROL_POINTS = BEZIER_ORDER + 1;

    static_assert(BEZIER_ORDER > 0);

    void Draw() override;
    void Update(float) override;
};