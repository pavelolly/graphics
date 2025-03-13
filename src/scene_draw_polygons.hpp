#pragma once

#include <deque>
#include <vector>
#include <memory>

#include "polygon_animation.hpp"
#include "gui.hpp"
#include "scene.hpp"


struct SceneDrawPolygons : Scene {
    // these must be deques so refs to the objects are always valid
    std::deque<std::shared_ptr<Polygon>> polygons;
    std::deque<PolygonAnimation> animations;
    
    GUI_InputBoxPanel input_box_panel;
    GUI_Toggle toggle_draw_polygon;

    Point *dragged_point = nullptr;
    
    Polygon drawn_polygon;

    bool paused = false;
    bool dragging = false;

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