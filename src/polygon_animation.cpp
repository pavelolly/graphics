#include "polygon_animation.hpp"


Point PolygonAnimation::InterpolatorStep(float dt) {
    float speed = moving_speed * 100 * dt;

    if (trajectory.IsPoint()) {
        return trajectory.GetPoint();
    }

    assert(trajectory.IsPolygon() && "trajectory is in invalid state");

    auto trajectory_ptr = trajectory.GetPolygon();
    if (trajectory_ptr.expired()) {
        return Vector2Zeros;
    }
    auto polygon_trajectory = trajectory_ptr.lock();

    float len = polygon_trajectory->Perimeter();
    int npoints = polygon_trajectory->NumPoints();

    if (npoints == 0) {
        return Vector2Zeros;
    }
    if (len == 0) {
        return polygon_trajectory->GetPoint(0);
    }

    float step_len = fmodf(speed, len);

    Point a = polygon_trajectory->GetPoint(trajectory_edge_idx % npoints);
    Point b = polygon_trajectory->GetPoint((trajectory_edge_idx + 1) % npoints);

    Point current_pos = Lerp(a, b, trajectory_edge_interpolator);

    while (step_len > 0) {
        float d = Distance(current_pos, b);

        if (d > step_len) {
            trajectory_edge_interpolator += step_len / Distance(a, b);
            break;
        }

        // switch to next edge
        step_len -= d;
        trajectory_edge_interpolator = 0;

        current_pos = b;
        a = b;
        b = polygon_trajectory->GetPoint((trajectory_edge_idx + 2) % npoints);

        trajectory_edge_idx = (trajectory_edge_idx + 1) % npoints;
    }

    return Lerp(a, b, trajectory_edge_interpolator);
}

void PolygonAnimation::Update(float dt) {
    animated_polygon->SetCenter(InterpolatorStep(dt));
    animated_polygon->Rotate(rotation_speed * dt);
}

void PolygonAnimation::Reset() {
    if (!original_polygon.expired()) {
        original_polygon.lock()->CloneInto(animated_polygon.get());
    }

    if (trajectory.IsPoint()) {
        trajectory.GetPoint() = original_point;
    }
    trajectory_edge_idx = 0.f;
    trajectory_edge_interpolator = 0.f;
}