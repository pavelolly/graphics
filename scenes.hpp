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

    Point InterpolatorStep(float dt) {
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

        float len   = polygon_trajectory->Length();
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

    void Update(float dt) {
        animated_polygon->SetCenter(InterpolatorStep(dt));
        animated_polygon->Rotate(rotation_speed * dt);
    }

    void Reset() {
        if (!original_polygon.expired())  {
            original_polygon.lock()->CloneInto(animated_polygon.get());
        }

        if (trajectory.IsPoint()) {
            trajectory.GetPoint() = original_point;
        }
        trajectory_edge_idx          = 0.f;
        trajectory_edge_interpolator = 0.f;
    }
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

    SceneEllipses() :
        ellipses {
            std::make_shared<Ellipse>( Point { GetScreenWidth() * 7.f / 16.f, GetScreenHeight() / 2.f }, 200, 100 ),
            std::make_shared<Ellipse>( ellipses[0]->GetPoint(0),                                         100, 50  ),
            std::make_shared<Ellipse>( ellipses[0]->GetPoint(0),                                         50,  25  )
        },
        animations {
            PolygonAnimation(ellipses[0]),
            PolygonAnimation(ellipses[1], animations[0].animated_polygon),
            PolygonAnimation(ellipses[2], animations[1].animated_polygon)
        }
    {
        // initial parameters
        animations[0].rotation_speed = 1;

        animations[1].rotation_speed = 2;
        animations[1].moving_speed   = 2;

        animations[2].rotation_speed = 3;
        animations[2].moving_speed   = 3;

        input_boxes[0] = GUI_InputBox(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + 0 * (BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, &animations[0].rotation_speed, "Rotation Speed 1\t");
        input_boxes[1] = GUI_InputBox(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + 1 * (BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, &animations[1].moving_speed,   "Moving Speed 2\t");
        input_boxes[2] = GUI_InputBox(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + 2 * (BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, &animations[1].rotation_speed, "Rotation Speed 2\t");
        input_boxes[3] = GUI_InputBox(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + 3 * (BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, &animations[2].moving_speed,   "Moving Speed 3\t");
        input_boxes[4] = GUI_InputBox(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + 4 * (BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, &animations[2].rotation_speed, "Rotation Speed 3\t");
    }

    bool IsSwitchable() override {
        for (auto &input_box : input_boxes) {
            if (input_box.editmode) {
                return false;
            }
        }
        return true;
    }

    void Draw() override {
        for (auto &animation : animations) {
            animation.animated_polygon->Draw(YELLOW, RED);
        }

        DrawRectangleRec(PANEL, GetColor(0x181818ff));
        DrawRectangleLinesEx(PANEL, GuiGetStyle(DEFAULT, BORDER_WIDTH), GRAY);
        for (auto &input_box : input_boxes) {
            input_box.Draw();
        }

        if (paused) {
            DrawText("paused", 20, 20, GuiGetStyle(DEFAULT, TEXT_SIZE), GRAY);
        }
    }

    void Update(float dt) override {
        if (IsKeyPressed(KEY_SPACE)) {
            paused = !paused;
        }

        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyPressed('R')) {
                for (auto &animation : animations) {
                    animation.Reset();

                    // if paused we want to see results of resetting immediately
                    if (paused) {
                        animation.Update(0);
                    }
                }
            
                for (auto &input_box : input_boxes) {
                    input_box.Reset();
                }
            }

        } else {
            if (IsKeyPressed('R')) {
                for (auto &animation : animations) {
                    animation.Reset();

                    // if paused we want to see results of resetting immediately
                    if (paused) {
                        animation.Update(0);
                    }
                }
            }
        }

        if (!paused) {
            for (auto &animation : animations) {
                animation.Update(dt);
            }
        }
    }
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

    void Draw() override {
        for (auto &animation : animations) {
            animation.animated_polygon->Draw(YELLOW, RED);
        }

        drawn_polygon.Draw(ORANGE, PURPLE);

        DrawRectangleRec(PANEL, GetColor(0x181818ff));
        DrawRectangleLinesEx(PANEL, GuiGetStyle(DEFAULT, BORDER_WIDTH), GRAY);
        for (auto &input_box : input_boxes) {
            input_box.Draw();
        }

        toggle_draw_polygon.Draw();

        if (paused) {
            DrawText("paused", 20, 20, GuiGetStyle(DEFAULT, TEXT_SIZE), GRAY);
        }
    }

    bool IsSwitchable() override {
        for (auto &input_box : input_boxes) {
            if (input_box.editmode) {
                return false;
            }
        }
        return true;
    }

    void Update(float dt) override {
        if (IsKeyPressed(KEY_SPACE)) {
            paused = !paused;
        }

        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyPressed('R')) {
                for (auto &animation : animations) {
                    animation.Reset();
                    // if paused we want to see results of resetting immediately
                    if (paused) {
                        animation.Update(0);
                    }
                }
            
                for (auto &input_box : input_boxes) {
                    input_box.Reset();
                }
            }

        } else {
            if (IsKeyPressed('R')) {
                for (auto &animation : animations) {
                    animation.Reset();
                    // if paused we want to see results of resetting immediately
                    if (paused) {
                        animation.Update(0);
                    }
                }
            }
        }

        if (toggle_draw_polygon.active) {

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Point mouse_pos = GetMousePosition();

                if (!CheckCollisionPointRec(mouse_pos, { PANEL_X, PANEL_Y, PANEL_W, PANEL_H })) {
                    drawn_polygon.AddPoint(mouse_pos);
                }
            }
        
        } else {

            if (drawn_polygon.NumPoints() != 0) {
                polygons.push_back(std::make_shared<Polygon>(std::move(drawn_polygon)));

                if (animations.size() == 0) {
                    animations.emplace_back(polygons.back());

                    AddInputBox(&animations[0].rotation_speed, "Rotation Speed 1\t");
                } else {
                    // move the original polygon to where it started the animation
                    // so resetting the animation puts it in the right place
                    polygons.back()->SetCenter(animations.back().animated_polygon->GetPoint(0));

                    animations.emplace_back(polygons.back(), animations.back().animated_polygon);

                    auto polygon_ordinal = std::to_string(animations.size());
                    AddInputBox(&animations.back().moving_speed,   "Moving Speed "   + polygon_ordinal + "\t");
                    AddInputBox(&animations.back().rotation_speed, "Rotation Speed " + polygon_ordinal + "\t");
                }
            }

        }

        if (paused) {

            if (dragging) {
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    assert(dragged_point && "dragged_point is nullptr when trying to drag");

                    *dragged_point += GetMouseDelta();
                    
                    // for (size_t i = 0; i < animations.size(); ++i) {
                    //     if (i == 0) {
                    //         animations[i].interpolator.default_point = animations[i].polygon->GetCenter();
                    //     } else {
                    //         animations[i].interpolator.default_point = animations[i - 1].polygon->GetPoint(0);
                    //     }
                    //     animations[i].Update(0);
                    // }
                }

                if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT)) {
                    dragging = false;
                    dragged_point = nullptr;
                }
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                Point mouse_pos = GetMousePosition();

                for (size_t i = 0; i < animations.size(); ++i) {
                    for (auto &point : animations[i].animated_polygon->vertexes) {
                        if (CheckCollisionPointCircle(mouse_pos, point, 10)) {
                            dragged_point = &point;
                            break;
                        }
                    }
                    if (dragged_point) {
                        dragging = true;
                        break;
                    }
                }
            }

        }

        if (IsKeyPressed(KEY_DELETE)) {
            polygons.clear();
            animations.clear();
            input_boxes.clear();
        }

        Point shift = Vector2Zeros;
        if (IsKeyDown(KEY_LEFT)) {
            shift += {-1, 0};
        }
        if (IsKeyDown(KEY_RIGHT)) {
            shift += {1, 0};
        }
        if (IsKeyDown(KEY_UP)) {
            shift += {0, -1};
        }
        if (IsKeyDown(KEY_DOWN)) {
            shift += {0, 1};
        }
        shift *= 200 * dt;

        if (shift != Vector2Zeros) {
            for (auto &animation : animations) {
                animation.animated_polygon->Shift(shift);
                if (animation.trajectory.IsPoint()) {
                    animation.trajectory.GetPoint() += shift;
                }
            }
            
            drawn_polygon.Shift(shift);
        }
        
        if (!paused) {
            for (auto &animation : animations) {
                animation.Update(dt);
            }
        }
    }
};

struct SceneLocalization : Scene {
    struct {
        Point a;
        Point b;
        Point c;
    } triangle;

    bool dragging = false;
    Point *dragged_point = nullptr;

    SceneLocalization() {
        auto w = GetScreenWidth();
        auto h = GetScreenHeight();

        auto GetRandomPoint = [w, h]() {
            return Point { static_cast<float>(GetRandomValue(-w / 2, w / 2)), static_cast<float>(GetRandomValue(-h / 2, h / 2)) };
        };

        triangle.a = GetScreenCenter() + GetRandomPoint();
        triangle.b = GetScreenCenter() + GetRandomPoint();
        triangle.c = GetScreenCenter() + GetRandomPoint();
    }

    void Draw() override {
        DrawLineV(triangle.a, triangle.b, YELLOW);
        DrawLineV(triangle.b, triangle.c, YELLOW);
        DrawLineV(triangle.c, triangle.a, YELLOW);

        DrawPoint(triangle.a, RED, 7);
        DrawPoint(triangle.b, RED, 7);
        DrawPoint(triangle.c, RED, 7);

        DrawPoint(GetMousePosition(),
                  IsInsideTriangle2(GetMousePosition(), triangle.a, triangle.b, triangle.c) ? GREEN : PURPLE,
                  15);
    }

    void Update(float dt) override {
        if (dragging) {
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                assert(dragged_point && "dragged_point is nullptr when trying to drag");

                *dragged_point += GetMouseDelta();
            }

            if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT)) {
                dragging = false;
                dragged_point = nullptr;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Point mouse_pos = GetMousePosition();

            if (CheckCollisionPointCircle(mouse_pos, triangle.a, 10)) {
                dragging = true;
                dragged_point = &triangle.a;
            }
            if (CheckCollisionPointCircle(mouse_pos, triangle.b, 10)) {
                dragging = true;
                dragged_point = &triangle.b;
            }
            if (CheckCollisionPointCircle(mouse_pos, triangle.c, 10)) {
                dragging = true;
                dragged_point = &triangle.c;
            }
        }
    }
};