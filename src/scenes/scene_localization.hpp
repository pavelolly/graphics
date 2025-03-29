#pragma once

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

    SceneLocalization();

    void Draw() override;
    void Update(float dt) override;
};