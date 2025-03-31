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
    struct BezierSet {
        std::deque<BezierCurve> curves;
        std::deque<Point> control_points;
    };

    std::deque<BezierSet> bezier_sets;

    PointDragger dragger;

    Camera2D camera = {0};

    bool show_control_points = true;
    bool need_new_set = true;

    static constexpr size_t BEZIER_ORDER = 2;
    static constexpr size_t ELEM_CONTROL_POINTS = BEZIER_ORDER + 1;

    static_assert(BEZIER_ORDER > 0);

    SceneBezier() {
        camera.zoom = 1;
        dragger.camera = &camera;
    }

    void Draw() override;
    void Update(float dt) override;

    void UpdateAllCurves();
};