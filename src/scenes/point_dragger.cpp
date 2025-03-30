#include "point_dragger.hpp"

#include <raylib.h>

#include <cassert>

std::optional<size_t> PointDragger::Update() {
    if (dragging) {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            assert(idx >= 0 && "dragged point is undefined when trying to drag");

            if (Point delta = GetMouseDelta(); delta != Vector2Zeros) {
                *points[idx] += delta;
                return idx;
            }
        }

        if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT)) {
            dragging = false;
            idx = -1;
        }

        return { std::nullopt };
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        TraceLog(LOG_DEBUG, "PointDragger: MOUSE_BUTTON_RIGHT Pressed");

        Point mouse_pos = GetMousePosition();

        for (int i = 0; i < static_cast<int>(points.size()); ++i) {
            if (CheckCollisionPointCircle(mouse_pos, *points[i], 10)) {
                idx = i;
                dragging = true;

                TraceLog(LOG_DEBUG, "PointDragger: Captured point %i", idx);
                break;
            }
        }
    }

    return { std::nullopt };
}