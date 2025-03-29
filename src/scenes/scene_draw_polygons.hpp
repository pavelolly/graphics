#pragma once

#include <deque>

#include "geometry/geometry.hpp"
#include "geometry/polygon_animation.hpp"
#include "scenes/point_dragger.hpp"
#include "scenes/scene.hpp"
#include "gui/gui.hpp"


struct SceneDrawPolygons : Scene {
    // these must be deques so refs to the objects are always valid
    std::deque<Polygon> polygons;
    std::deque<PolygonAnimation> animations;
    
    GUI::InputBoxPanel input_box_panel;
    GUI::Toggle toggle_draw_polygon;
    
    Polygon drawn_polygon;

    bool paused = false;

    PointDragger dragger;

    SceneDrawPolygons() :
        input_box_panel({ GetScreenWidth() - 400.f, 40, 360, GetScreenHeight() - 80.f }),
        toggle_draw_polygon(Rectangle{ 
                input_box_panel.panel.x + input_box_panel.panel.width - input_box_panel.DEFAULT_BOX_WIDTH - 35,
                input_box_panel.panel.y + input_box_panel.panel.height - 2 * input_box_panel.DEFAULT_BOX_HEIGHT,
                input_box_panel.DEFAULT_BOX_WIDTH,
                input_box_panel.DEFAULT_BOX_HEIGHT },
            "Draw", "Finish")
    {}

    bool IsSwitchable() override;
    void Draw() override;
    void Update(float dt) override;
};