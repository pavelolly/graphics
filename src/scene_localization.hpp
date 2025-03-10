#pragma once

#include "geometry.hpp"
#include "scene.hpp"

struct SceneLocalization : Scene {
    struct {
        Point a;
        Point b;
        Point c;
    } triangle;

    bool dragging = false;
    Point *dragged_point = nullptr;

    SceneLocalization();

    void Draw() override;
    void Update(float dt) override;
};