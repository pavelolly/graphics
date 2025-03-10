#pragma once

#include <variant>
#include <memory>

#include "geometry.hpp"

// simple wrapper around std::variant for convinience
// Trajectory can be reference to polygon or single point
struct Trajectory {
    using PolygonPtr = std::weak_ptr<const Polygon>;

    std::variant<PolygonPtr, Point> trajectory;

    Trajectory() = default;
    Trajectory(PolygonPtr polygon) : trajectory(polygon) {}
    Trajectory(Point point) : trajectory(point) {}

    bool IsPolygon() const {
        return std::holds_alternative<PolygonPtr>(trajectory);
    }
    bool IsPoint() const {
        return std::holds_alternative<Point>(trajectory);
    }

    PolygonPtr GetPolygon() const {
        return std::get<PolygonPtr>(trajectory);
    }
    Point& GetPoint() {
        return std::get<Point>(trajectory);
    }
    Point GetPoint() const {
        return std::get<Point>(trajectory);
    }
};

struct PolygonAnimation {
    // reference to orignal polygon that is animated
    std::weak_ptr<const Polygon> original_polygon;

    // original position of polygon if no trajectory-as-polygon was introduced
    Point original_point = Vector2Zeros;

    // copy of original polygon that is actually changed during animation
    std::shared_ptr<Polygon> animated_polygon;
    
    Trajectory trajectory;
    size_t trajectory_edge_idx         = 0;   // edge of trajectory we're currently at
    float trajectory_edge_interpolator = 0.f; // interpolator for the current edge
    
    // animation parameters
    float moving_speed   = 0.f;
    float rotation_speed = 0.f;

    PolygonAnimation() = default;

    template <typename Poly> requires std::is_base_of_v<Polygon, Poly>
    PolygonAnimation(std::shared_ptr<Poly> polygon, std::shared_ptr<Polygon> trajectory) :
        original_polygon(polygon),
        animated_polygon(std::make_shared<Poly>(*polygon)),
        trajectory(trajectory)
    {}

    template <typename Poly> requires std::is_base_of_v<Polygon, Poly>
    PolygonAnimation(std::shared_ptr<Poly> polygon) :
        original_polygon(polygon),
        original_point(polygon->GetCenter()),
        animated_polygon(std::make_shared<Poly>(*polygon)),
        trajectory(original_point)
    {}

    Point InterpolatorStep(float dt);
    void Update(float dt);
    void Reset();
};
