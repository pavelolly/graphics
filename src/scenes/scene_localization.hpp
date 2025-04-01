#pragma once

#include <array>

#include "geometry/geometry.hpp"
#include "scenes/point_dragger.hpp"
#include "scenes/scene.hpp"

struct SceneLocalization : Scene {
    struct {
        Point a;
        Point b;
        Point c;
    } triangle;

    PointDragger dragger;

    static constexpr auto is_inside_funcs = std::array{ IsInsideTriangle, IsInsideTriangle2, IsInsideTriangle3 };
    int mode = 0; // idx of is_inside_func

    SceneLocalization();

    void Draw() override;
    void Update(float dt) override;
};