#include "point_dragger.hpp"

#include <raylib.h>

#include <cassert>

std::optional<size_t> PointDragger::Update() {
    if (dragging) {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            assert(idx >= 0 && "dragged point is undefined when trying to drag");

            if (Point delta = GetMouseDelta(); delta != Vector2Zeros) {
                if (camera.has_value()) {
                    delta *= 1.f / camera.value()->zoom;
                }
                *points[idx] += delta;
                return idx;
            }
        }

        if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT)) {
            dragging = false;
            idx = -1;
        }

        return std::nullopt;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        TraceLog(LOG_DEBUG, "PointDragger: MOUSE_BUTTON_RIGHT Pressed");

        Point mouse_pos = GetMousePosition();

        for (int i = 0; (size_t) i < points.size(); ++i) {
            Point point_pos_on_screen = *points[i];
            if (camera.has_value()) {
                point_pos_on_screen = GetWorldToScreen2D(*points[i], *camera.value());
            }
            if (CheckCollisionPointCircle(mouse_pos, point_pos_on_screen, 10)) {
                idx = i;
                dragging = true;

                TraceLog(LOG_DEBUG, "PointDragger: Captured point %i", idx);
                break;
            }
        }
    }

    return std::nullopt;
}