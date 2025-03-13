#pragma once

#include <array>

#include "geometry.hpp"
#include "gui.hpp"
#include "polygon_animation.hpp"
#include "scene.hpp"


struct SceneEllipses : Scene {
    std::array<std::shared_ptr<Ellipse>, 3> ellipses;
    std::array<PolygonAnimation, 3> animations;
    
    GUI::InputBoxPanel input_box_panel;

    bool paused = false;

    SceneEllipses();

    bool IsSwitchable() override;
    void Draw() override;
    void Update(float dt) override;
};