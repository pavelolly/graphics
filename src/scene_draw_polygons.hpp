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

    bool IsSwitchable() override;
    void Draw() override;
    void Update(float dt) override;
};