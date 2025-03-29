#include "point_dragger.hpp"

#include <raylib.h>

bool PointDragger::Update() {
    if (dragging) {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            assert(idx >= 0 && "dragged point is undefined when trying to drag");

            *points[idx] += GetMouseDelta();
            return true;
        }

        if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT)) {
            dragging = false;
            idx = -1;
        }

        return false;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        TraceLog(LOG_DEBUG, "MOUSE_BUTTON_RIGHT Pressed");

        Point mouse_pos = GetMousePosition();

        for (int i = 0; i < points.size(); ++i) {
            if (CheckCollisionPointCircle(mouse_pos, *points[i], 10)) {
                idx = i;
                dragging = true;

                TraceLog(LOG_DEBUG, "Captured point %i", idx);
                break;
            }
        }
    }

    return false;
}