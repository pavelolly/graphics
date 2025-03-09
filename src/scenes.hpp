#pragma once

#include <raylib.h>

#include <array>
#include <deque>
#include <memory>
#include <variant>

#include "geometry.hpp"
#include "gui.hpp"

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

struct Scene {
    virtual void Draw()           = 0;
    virtual void Update(float dt) = 0;
    virtual bool IsSwitchable() { return true; }
    virtual ~Scene() = default;
};

// TODO: do smth with this defines
//       they are kind of... odd
#define PANEL_X  (static_cast<float>(GetScreenWidth()) - 400) // static_cast bruh...
#define PANEL_Y  40
#define PANEL_W  400
#define PANEL_H  (static_cast<float>(GetScreenHeight()) - 40) // static_cast bruh...

#define PANEL (Rectangle { PANEL_X - 10, PANEL_Y - 10, PANEL_W - 10, PANEL_H - 10 })

#define BUTTON_W 80
#define BUTTON_H 30


struct SceneEllipses : Scene {
    std::array<std::shared_ptr<Ellipse>, 3> ellipses;
    std::array<PolygonAnimation, 3> animations;
    std::array<GUI_InputBox,     5> input_boxes;

    bool paused = false;

    SceneEllipses();

    bool IsSwitchable() override;
    void Draw() override;
    void Update(float dt) override;
};


struct SceneDrawPolygons : Scene {
    // these must be deques so refs to the objects are always valid
    std::deque<std::shared_ptr<Polygon>> polygons;
    std::deque<PolygonAnimation> animations;
    
    std::vector<GUI_InputBox> input_boxes;
    GUI_Toggle toggle_draw_polygon;

    Point *dragged_point = nullptr;
    
    Polygon drawn_polygon;

    bool paused = false;
    bool dragging = false;

    SceneDrawPolygons() :
        toggle_draw_polygon(Rectangle{ PANEL_X + PANEL_W - BUTTON_W - 35, PANEL_Y + PANEL_H - 2 * BUTTON_H, BUTTON_W, BUTTON_H }, "Draw", "Finish")
    {}

    void AddInputBox(float *value, std::string text="") {
        size_t nbuttons = input_boxes.size();
        input_boxes.emplace_back(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + nbuttons*(BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, value, std::move(text));
    }

    bool IsSwitchable() override;
    void Draw() override;
    void Update(float dt) override;
};

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
    void Update(float) override;
};