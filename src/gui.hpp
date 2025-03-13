#pragma once

#define RAYGUI_VALUEBOX_MAX_CHARS 10
#include <raygui.h>

#include <vector>
#include <string>

namespace GUI {

struct InputBox {
    Rectangle box;
    float* value = nullptr;
    std::string text;
    char text_buffer[RAYGUI_VALUEBOX_MAX_CHARS + 1] = {0};
    bool editmode = false;

    InputBox() = default;
    InputBox(Rectangle box, float *value, std::string text);

    void UpdateTextBuffer();
    void Reset();
    void Draw();
};

struct InputBoxPanel {
    Rectangle panel;
    std::vector<GUI::InputBox> input_boxes;

    static const int DEFAULT_BOX_WIDTH   = 80;
    static const int DEFAULT_BOX_HEIGHT  = 40;
    static const int DEFAULT_BOX_PADDING = 10;
    static const int DEFAULT_MARGIN      = 20;

    InputBoxPanel() = default;
    InputBoxPanel(Rectangle box) : panel(box) {}

    void Add(float *value, std::string text);

    void Draw();
    void Reset();
};

struct Toggle {
    Rectangle box;
    std::string text_inactive;
    std::string text_active;
    bool active = false;

    Toggle() = default;

    Toggle(Rectangle box, std::string text_inactive, std::string text_active) :
        box(box), text_inactive(std::move(text_inactive)), text_active(std::move(text_active))
    {}

    void Draw() {
        GuiToggle(box, active ? text_active.c_str() : text_inactive.c_str(), &active);
    }
};

} // namespace GUI
