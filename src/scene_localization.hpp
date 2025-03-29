#pragma once

#include "geometry.hpp"
#include "point_dragger.hpp"
#include "scene.hpp"

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