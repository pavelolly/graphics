#pragma once

#include <array>

#include "geometry/geometry.hpp"
#include "geometry/polygon_animation.hpp"
#include "gui/gui.hpp"
#include "scenes/scene.hpp"


struct SceneEllipses : Scene {
    std::array<Polygon, 3> ellipses;
    std::array<PolygonAnimation, 3> animations;
    
    GUI::InputBoxPanel input_box_panel;

    bool paused = false;

    SceneEllipses();

    bool IsSwitchable() override;
    void Draw() override;
    void Update(float dt) override;
};