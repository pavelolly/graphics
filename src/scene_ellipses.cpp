#include "scene_ellipses.hpp"


SceneEllipses::SceneEllipses() : input_box_panel(Rectangle { GetScreenWidth() - 400.f, 40, 360, GetScreenHeight() - 80.f } )
{
    ellipses[0] = std::make_shared<Ellipse>( Point { GetScreenWidth() * 7.f / 16.f, GetScreenHeight() / 2.f }, 200.f, 100.f );
    ellipses[1] = std::make_shared<Ellipse>( ellipses[0]->GetPoint(0),                                         100.f, 50.f  );
    ellipses[2] = std::make_shared<Ellipse>( ellipses[1]->GetPoint(0),                                         50.f,  25.f  );

    animations[0] = PolygonAnimation(ellipses[0]);
    animations[0].rotation_speed = 1;

    animations[1] = PolygonAnimation(ellipses[1], animations[0].animated_polygon);
    animations[1].rotation_speed = 2;
    animations[1].moving_speed   = 2;

    animations[2] = PolygonAnimation(ellipses[2], animations[1].animated_polygon);
    animations[2].rotation_speed = 3;
    animations[2].moving_speed   = 3;
    
    input_box_panel.Add(&animations[0].rotation_speed, "Rotation Speed 1");
    input_box_panel.Add(&animations[1].moving_speed,   "Moving Speed 2");
    input_box_panel.Add(&animations[1].rotation_speed, "Rotation Speed 2");
    input_box_panel.Add(&animations[2].moving_speed,   "Moving Speed 3");
    input_box_panel.Add(&animations[2].rotation_speed, "Rotation Speed 3");
}

bool SceneEllipses::IsSwitchable() {
    for (auto &input_box : input_box_panel.input_boxes) {
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

    input_box_panel.Draw();

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

    if (!paused) {
        for (auto &animation : animations) {
            animation.Update(dt);
        }
    }
}