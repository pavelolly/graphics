#pragma once

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_VALUEBOX_MAX_CHARS 10
#include <raygui.h>

#include <vector>
#include <string>
#include <algorithm>

#include "log_macros.h"

struct GUI_InputBox {
    Rectangle box;
    float* value;
    std::string text;
    char text_buffer[RAYGUI_VALUEBOX_MAX_CHARS + 1] = {0};
    bool editmode = false;

    GUI_InputBox(Rectangle box, float *value, std::string text) : box(box), value(value), text(std::move(text)) {
        if (*value != 0) {
            snprintf(text_buffer, RAYGUI_VALUEBOX_MAX_CHARS, "%.2f", *value);
        }
    }

    void Reset() {
        memset(text_buffer, 0, sizeof(text_buffer));
        *value = 0;
    }

    void Draw() {
        if (GuiValueBoxFloat(box, text.c_str(), text_buffer, value, editmode)) {
            editmode = !editmode;
        }
    }
};

struct GUI_Toggle {
    Rectangle box;
    std::string text_inactive;
    std::string text_active;
    bool active = false;

    GUI_Toggle(Rectangle box, std::string text_inactive, std::string text_active) :
        box(box), text_inactive(std::move(text_inactive)), text_active(std::move(text_active))
    {}

    void Draw() {
        GuiToggle(box, active ? text_active.c_str() : text_inactive.c_str(), &active);
    }
};