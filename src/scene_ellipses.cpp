#include "scenes.hpp"


SceneEllipses::SceneEllipses() {
    ellipses[0] = std::make_shared<Ellipse>( Point { GetScreenWidth() * 7.f / 16.f, GetScreenHeight() / 2.f }, 200, 100 );
    ellipses[1] = std::make_shared<Ellipse>( ellipses[0]->GetPoint(0),                                         100, 50  );
    ellipses[2] = std::make_shared<Ellipse>( ellipses[1]->GetPoint(0),                                         50,  25  );

    animations[0] = PolygonAnimation(ellipses[0]);
    animations[0].rotation_speed = 1;

    animations[1] = PolygonAnimation(ellipses[1], animations[0].animated_polygon);
    animations[1].rotation_speed = 2;
    animations[1].moving_speed   = 2;

    animations[2] = PolygonAnimation(ellipses[2], animations[1].animated_polygon);
    animations[2].rotation_speed = 3;
    animations[2].moving_speed   = 3;

    input_boxes[0] = GUI_InputBox(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + 0 * (BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, &animations[0].rotation_speed, "Rotation Speed 1\t");
    input_boxes[1] = GUI_InputBox(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + 1 * (BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, &animations[1].moving_speed,   "Moving Speed 2\t");
    input_boxes[2] = GUI_InputBox(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + 2 * (BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, &animations[1].rotation_speed, "Rotation Speed 2\t");
    input_boxes[3] = GUI_InputBox(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + 3 * (BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, &animations[2].moving_speed,   "Moving Speed 3\t");
    input_boxes[4] = GUI_InputBox(Rectangle { PANEL_X + PANEL_W / 2, PANEL_Y + 4 * (BUTTON_H + 10.f), BUTTON_W, BUTTON_H }, &animations[2].rotation_speed, "Rotation Speed 3\t");
}

bool SceneEllipses::IsSwitchable() {
    for (auto &input_box : input_boxes) {
        if (input_box.editmode) {
            return false;
        }
    }
    return true;
}

void SceneEllipses::Draw() {
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

void SceneEllipses::Update(float dt) {
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