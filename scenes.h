#pragma once

#include <raylib.h>

#include <array>

#include "geometry.h"
#include "gui.h"

struct PolygonAnimation {
    Polygon *polygon;
    Interpolator interpolator;
    float rotation = 0;
    float moving_speed = 0;
    float rotation_speed = 0;

    PolygonAnimation(Polygon &polygon, const Polygon &trajectory) :
        polygon(&polygon), interpolator(trajectory)
    {}

    PolygonAnimation(Polygon &polygon) :
        polygon(&polygon), interpolator(polygon.GetCenter())
    {}

    void Update(float dt) {
        polygon->SetCenter(interpolator.Step(moving_speed * 100 * dt));
        
        polygon->Rotate(rotation_speed * dt);
        rotation = fmodf(rotation + rotation_speed * dt, 2 * std::numbers::pi);
    }

    void Reset() {
        polygon->Rotate(-rotation);
        rotation = 0;
        polygon->SetCenter(interpolator.default_point);

        interpolator.Reset();
    }
};

struct Scene {
    bool switchable = true;

    virtual void Draw()           = 0;
    virtual void Update(float dt) = 0;
    virtual ~Scene() {};
};


#define PANEL_X  (static_cast<float>(GetScreenWidth()) - 400) // static_cast bruh...
#define PANEL_Y  40
#define PANEL_W  400
#define PANEL_H  (static_cast<float>(GetScreenHeight()) - 40) // static_cast bruh...

#define PANEL (Rectangle { PANEL_X - 10, PANEL_Y - 10, PANEL_W - 10, PANEL_H - 10 })

#define BUTTON_W 80
#define BUTTON_H 30


struct SceneEllipses : Scene {
    std::array<Ellipse, 3> ellipses;
    std::array<PolygonAnimation, 3> animations;
    std::array<GUI_InputBox, 5> input_boxes;

    bool paused = false;

    SceneEllipses() :
        ellipses {
            Ellipse( { GetScreenWidth() * 7.f / 16.f, GetScreenHeight() / 2.f }, 200, 100 ),
            Ellipse( ellipses[0].GetPoint(0),                                    100, 50  ),
            Ellipse( ellipses[0].GetPoint(0),                                    50,  25  )
        },
        animations {
            PolygonAnimation(ellipses[0]),
            PolygonAnimation(ellipses[1], ellipses[0]),
            PolygonAnimation(ellipses[2], ellipses[1])
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

    void Draw() override {
        for (auto &animation : animations) {
            animation.polygon->Draw(YELLOW, RED);
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
                }
            
                for (auto &input_box : input_boxes) {
                    input_box.Reset();
                }
            }

        } else {
            if (IsKeyPressed('R')) {
                for (auto &animation : animations) {
                    animation.Reset();
                }
            }
        }
        
        for (auto &animation : animations) {
            animation.Update(dt);
        }

        switchable = true;
        for (auto &input_box : input_boxes) {
            if (input_box.editmode) {
                switchable = false;
                break;
            }
        }
    }
};


struct SceneDrawPolygons : Scene {
    // these muse be deques so refs to these objects are always valid
    std::deque<Polygon> polygons;
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

    void AddAnimation(const PolygonAnimation &animation) {
        animations.push_back(animation);
    }

    void AddInputBox(float *value, std::string text="") {
        size_t nbuttons = input_boxes.size();
        input_boxes.emplace_back(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + nbuttons*(BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, value, std::move(text));
    }

    void Draw() override {
        for (auto &animation : animations) {
            animation.polygon->Draw(YELLOW, RED);
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

    void Update(float dt) override {
        if (IsKeyPressed(KEY_SPACE)) {
            paused = !paused;
        }

        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyPressed('R')) {
                for (auto &animation : animations) {
                    animation.Reset();
                }
            
                for (auto &input_box : input_boxes) {
                    input_box.Reset();
                }
            }

        } else {
            if (IsKeyPressed('R')) {
                for (auto &animation : animations) {
                    animation.Reset();
                }
            }
        }

        switchable = true;
        for (auto &input_box : input_boxes) {
            if (input_box.editmode) {
                switchable = false;
                break;
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
                polygons.push_back(std::move(drawn_polygon));

                if (animations.size() == 0) {
                    animations.push_back(PolygonAnimation(polygons.back()));

                    AddInputBox(&animations[0].rotation_speed, "Rotation Speed 1\t");
                } else {
                    animations.push_back(PolygonAnimation(polygons.back(), *animations.back().polygon));
                    animations.back().polygon->SetCenter(animations.back().interpolator.default_point);

                    int polygon_ordinal = animations.size();
                    AddInputBox(&animations[polygon_ordinal - 1].moving_speed,   "Moving Speed "   + std::to_string(polygon_ordinal) + "\t");
                    AddInputBox(&animations[polygon_ordinal - 1].rotation_speed, "Rotation Speed " + std::to_string(polygon_ordinal) + "\t");
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
                    for (auto &point : animations[i].polygon->vertexes) {
                        if (Distance(mouse_pos, point) < 10) {
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
            shift += {-200, 0};
        }
        if (IsKeyDown(KEY_RIGHT)) {
            shift += {200, 0};
        }
        if (IsKeyDown(KEY_UP)) {
            shift += {0, -200};
        }
        if (IsKeyDown(KEY_DOWN)) {
            shift += {0, 200};
        }
        shift *= dt;

        if (shift != Vector2Zeros) {
            for (size_t i = 0; i < animations.size(); ++i) {
                animations[i].polygon->Shift(shift);
                if (animations[i].interpolator.polygon == nullptr) {
                    animations[i].interpolator.default_point = animations[i].polygon->GetCenter();
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

            if (Distance(triangle.a, mouse_pos) < 10) {
                dragging = true;
                dragged_point = &triangle.a;
            }
            if (Distance(triangle.b, mouse_pos) < 10) {
                dragging = true;
                dragged_point = &triangle.b;
            }
            if (Distance(triangle.c, mouse_pos) < 10) {
                dragging = true;
                dragged_point = &triangle.c;
            }
        }
    }
};