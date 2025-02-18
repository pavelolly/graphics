
#include <raylib.h>

#include <deque>

#include "geometry.h"
#include "gui.h"

#include "log_macros.h"

struct PolygonStorage {
    std::vector<Polygon *> polygons;

    ~PolygonStorage() {
        for (auto p : polygons) {
            delete p;
        }
    }

    template <typename Poly>
    Polygon *Add(const Poly &polygon) {
        Polygon *_new = new Poly(polygon);
        polygons.push_back(_new);
        return _new;
    }

    void Remove(Polygon *polygon) {
        std::erase(polygons, polygon);
        delete polygon;
    }
};

struct PolygonAnimation {
    Polygon *polygon;
    Interpolator interpolator;
    float rotation = 0;
    float moving_speed = 0;
    float rotation_speed = 0;

    PolygonAnimation(Polygon &polygon, const Polygon &trajectory) :
        polygon(&polygon), interpolator(&trajectory)
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

        interpolator.Reset();
    }
};

#define WIDTH  1600
#define HEIGHT 900

#define PANEL_X  (WIDTH - 400)
#define PANEL_Y  40
#define PANEL_W  400
#define PANEL_H  (HEIGHT - 40)

#define PANEL (Rectangle { PANEL_X - 10, PANEL_Y - 10, PANEL_W - 10, PANEL_H - 10 })

#define BUTTON_W 80
#define BUTTON_H 30

struct Scene {
    enum {
        ELLIPSES,
        DRAW_POLYGONS
    } type;

    std::deque<PolygonAnimation> animations;
    std::vector<GUI_InputBox> input_boxes;

    void AddAnimation(const PolygonAnimation &animation) {
        animations.push_back(animation);
    }

    void AddInputBox(float *value, std::string text="") {
        size_t nbuttons = input_boxes.size();
        input_boxes.emplace_back(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + nbuttons*(BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, value, std::move(text));
    }

    void Draw() {
        for (auto &animation : animations) {
            animation.polygon->Draw(YELLOW, RED);
        }
    }

    void Shift(Point shift) {
        for (size_t i = 0; i < animations.size(); ++i) {
            animations[i].polygon->Shift(shift);
            if (animations[i].interpolator.polygon == nullptr) {
                animations[i].interpolator.default_point = animations[i].polygon->GetCenter();
            }
        }
    }

    void Update(float dt) {
        for (auto &animation : animations) {
            animation.Update(dt);
        }
    }

    void ResetParameters() {
        for (auto &input_box : input_boxes) {
            input_box.Reset();
        }
    }

    void ResetAnimations() {
        for (auto &animation : animations) {
            animation.Reset();
        }
    }
};

PolygonStorage polygons;

Scene GetEllipsesScene() {
    Polygon *eliipse1 = polygons.Add(Ellipse( { GetScreenWidth() * 7.f / 16.f, GetScreenHeight() / 2.f }, 200, 100 ));
    Polygon *eliipse2 = polygons.Add(Ellipse( eliipse1->GetPoint(0),                                      100, 50  ));
    Polygon *eliipse3 = polygons.Add(Ellipse( eliipse2->GetPoint(0),                                      50,  25  ));

    Scene scene;
    scene.type = Scene::ELLIPSES;
    scene.AddAnimation(PolygonAnimation(*eliipse1));
    scene.animations[0].rotation_speed = 1;

    scene.AddAnimation(PolygonAnimation(*eliipse2, *eliipse1));
    scene.animations[1].rotation_speed = 2;
    scene.animations[1].moving_speed   = 2;

    scene.AddAnimation(PolygonAnimation(*eliipse3, *eliipse2));
    scene.animations[2].rotation_speed = 3;
    scene.animations[2].moving_speed   = 3;

    scene.AddInputBox(&scene.animations[0].rotation_speed, "Rotation Speed 1\t");
    scene.AddInputBox(&scene.animations[1].moving_speed,   "Moving Speed 2\t");
    scene.AddInputBox(&scene.animations[1].rotation_speed, "Rotation Speed 2\t");
    scene.AddInputBox(&scene.animations[2].moving_speed,   "Moving Speed 3\t");
    scene.AddInputBox(&scene.animations[2].rotation_speed, "Rotation Speed 3\t");

    return scene;
}

Scene GetDrawPolygonsScene() {
    Scene scene;
    scene.type = Scene::DRAW_POLYGONS;

    // nothing special here yet as this scene is sort of dynamic

    return scene;
}

int main() {
    InitWindow(WIDTH, HEIGHT, "Ellipses");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_DEBUG);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);

    Scene scene_ellipses      = GetEllipsesScene();

    Scene scene_draw_polygons = GetDrawPolygonsScene();
    GUI_Toggle toggle_draw_polygon(Rectangle{ PANEL_X + PANEL_W - BUTTON_W - 35, PANEL_Y + PANEL_H - 2 * BUTTON_H, BUTTON_W, BUTTON_H }, "Draw", "Finish");
    Polygon *drawn_polygon = polygons.Add(Polygon{});
    bool dragging        = false;
    Point *dragged_point = nullptr;

    Scene *scene = &scene_ellipses;

    bool pause = false;

    while (!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(GetColor(0x181818ff));

            // Draw Polygons

            scene->Draw();
            if (scene->type == Scene::DRAW_POLYGONS) {
                drawn_polygon->Draw(ORANGE, PURPLE);
            }

            // Draw UI

            DrawRectangleRec(PANEL, GetColor(0x181818ff));
            DrawRectangleLinesEx(PANEL, GuiGetStyle(DEFAULT, BORDER_WIDTH), GRAY);
            for (auto &input_box : scene->input_boxes) {
                input_box.Draw();
            }
            if (scene->type == Scene::DRAW_POLYGONS) {
                toggle_draw_polygon.Draw();
            }

            if (pause) {
                DrawText("paused", 20, 20, GuiGetStyle(DEFAULT, TEXT_SIZE), GRAY);
            }

        EndDrawing();

        float dt = GetFrameTime();

        // Handle Reset

        if (IsKeyDown(KEY_LEFT_CONTROL)) {

            if (IsKeyPressed('R')) {
                scene->ResetAnimations();
                scene->ResetParameters();
            }

        } else {
            if (IsKeyPressed('R')) {
                scene->ResetAnimations();
            }
        }

        // Handle Switch

        if (IsKeyPressed('Q')) {
            scene = &scene_ellipses;
        }

        if (IsKeyPressed('E')) {
            scene = &scene_draw_polygons;
        }

        // Handle pause

        if (IsKeyPressed(KEY_SPACE)) {
            pause = !pause;
        } 

        // Handle Drawing

        if (scene->type == Scene::DRAW_POLYGONS) {
            
            if (toggle_draw_polygon.active) {

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    Point mouse_pos = GetMousePosition();

                    if (!CheckCollisionPointRec(mouse_pos, { PANEL_X, PANEL_Y, PANEL_W, PANEL_H })) {
                        drawn_polygon->AddPoint(GetMousePosition());
                    }
                }
            
            } else {

                if (drawn_polygon->NumPoints() != 0) {
                    if (scene->animations.size() == 0) {
                        scene->AddAnimation(PolygonAnimation(*drawn_polygon));

                        scene->AddInputBox(&scene->animations[0].rotation_speed, "Rotation Speed 1\t");
                    } else {
                        scene->AddAnimation(PolygonAnimation(*drawn_polygon, *scene->animations.back().polygon));

                        int polygon_ordinal = scene->animations.size();
                        scene->AddInputBox(&scene->animations[polygon_ordinal - 1].moving_speed,   "Moving Speed "   + std::to_string(polygon_ordinal) + "\t");
                        scene->AddInputBox(&scene->animations[polygon_ordinal - 1].rotation_speed, "Rotation Speed " + std::to_string(polygon_ordinal) + "\t");
                    }
                
                    drawn_polygon = polygons.Add(Polygon{});
                }

            }

            if (pause) {

                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    Point mouse_pos = GetMousePosition();

                    for (auto &animation : scene->animations) {
                        for (auto &point : animation.polygon->vertexes) {
                            if (Distance(mouse_pos, point) < 10) {
                                dragged_point = &point;
                                break;
                            }
                        }
                        if (dragged_point) {
                            break;
                        }
                    }

                    if (dragged_point) {
                        dragging = true;
                    }
                }

                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {

                    if (dragging) {
                        assert(dragged_point && "dragged_point is nullptr when trying to drag");
                        
                        *dragged_point += GetMouseDelta();
                    }

                } else if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT)) {

                    if (dragging) {
                        dragging = false;
                        dragged_point = nullptr;
                    }

                }

            }
        }

        // Handle Deletion Drawings

        if (IsKeyPressed(KEY_DELETE)) {
            if (scene->type == Scene::DRAW_POLYGONS) {
                scene->animations.clear();
                scene->input_boxes.clear();
            }
        }

        // Handle Shifting the scene

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
            scene->Shift(shift);
            if (scene->type == Scene::DRAW_POLYGONS) {
                drawn_polygon->Shift(shift);
            }
        }

        // Update

        if (!pause) {
            scene->Update(dt);
        }
    }

    CloseWindow();

    return 0;
}