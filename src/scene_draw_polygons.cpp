#include "scene_draw_polygons.hpp"


bool SceneDrawPolygons::IsSwitchable() {
    for (auto &input_box : input_box_panel.input_boxes) {
        if (input_box.editmode) {
            return false;
        }
    }
    return true;
}

void SceneDrawPolygons::Draw() {
    for (auto &animation : animations) {
        animation.animated_polygon->Draw(YELLOW, RED);
    }

    drawn_polygon.Draw(ORANGE, PURPLE);
    input_box_panel.Draw();
    toggle_draw_polygon.Draw();

    if (paused) {
        DrawText("paused", 20, 20, GuiGetStyle(DEFAULT, TEXT_SIZE), GRAY);
    }
}

void SceneDrawPolygons::Update(float dt) {
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

            input_box_panel.Reset();
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

            if (!CheckCollisionPointRec(mouse_pos, input_box_panel.panel)) {
                drawn_polygon.AddPoint(mouse_pos);
            }
        }

    } else {

        if (drawn_polygon.NumPoints() != 0) {
            polygons.push_back(std::make_shared<Polygon>(std::move(drawn_polygon)));

            if (animations.size() == 0) {
                animations.emplace_back(polygons.back());

                input_box_panel.Add(&animations[0].rotation_speed, "Rotation Speed 1");
            } else {
                // move the original polygon to where it started the animation
                // so resetting the animation puts it in the right place
                polygons.back()->SetCenter(animations.back().animated_polygon->GetPoint(0));

                animations.emplace_back(polygons.back(), animations.back().animated_polygon);

                auto polygon_ordinal = std::to_string(animations.size());
                input_box_panel.Add(&animations.back().moving_speed, "Moving Speed " + polygon_ordinal);
                input_box_panel.Add(&animations.back().rotation_speed, "Rotation Speed " + polygon_ordinal);
            }
        }
    }

    if (paused) {

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
        input_box_panel.input_boxes.clear();
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