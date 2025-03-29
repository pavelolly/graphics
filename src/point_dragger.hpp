#pragma once

#include <vector>
#include <ranges>

#include "geometry.hpp"

struct PointDragger {
    std::vector<Point *> points;
    int idx = -1; // index of dragged point
    bool dragging = false;

    void Clear() {
        points.clear();
        idx = -1;
        dragging = false;
    }

    void AddToDrag(Point &point) {
        points.push_back(&point);
    }
    void AddToDrag(std::ranges::range auto &points) {
        using std::ranges::begin;
        using std::ranges::end;

        for (auto it = begin(points), ite = end(points); it != ite; ++it) {
            this->points.push_back(&(*it));
        }
    }

    void Update();
};