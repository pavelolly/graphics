#pragma once

#include <vector>
#include <optional>

#include "geometry/geometry.hpp"

// struct that is used to mvoe points on a screen with MOUSE_BUTTON_RIGHT
struct PointDragger {
    std::vector<Point *> points;
    int idx = -1; // index of dragged point
    bool dragging = false;

    std::optional<Camera2D*> camera;

    void Clear() {
        points.clear();
        idx = -1;
        dragging = false;
    }

    void AddToDrag(Point &point) {
        points.push_back(&point);
    }
    // accepts any range of Point
    void AddToDrag(RangeOf<Point> auto &points) {
        using std::ranges::begin;
        using std::ranges::end;

        for (auto it = begin(points), ite = end(points); it != ite; ++it) {
            this->points.push_back(&(*it));
        }
    }

    std::optional<size_t> Update();
};