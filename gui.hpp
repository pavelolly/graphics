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
    float* value = nullptr;
    std::string text;
    char text_buffer[RAYGUI_VALUEBOX_MAX_CHARS + 1] = {0};
    bool editmode = false;

    GUI_InputBox() = default;

    GUI_InputBox(Rectangle box, float *value, std::string text) : box(box), value(value), text(std::move(text)) {
        if (value) {
            UpdateTextBuffer();
        }
    }

    void UpdateTextBuffer() {
        int len = snprintf(text_buffer, RAYGUI_VALUEBOX_MAX_CHARS + 1, "%f", *value);
        int dot = -1;
        for (int i = 0; i < len; ++i) {
            if (text_buffer[i] == '.') {
                dot = i;
                break;
            }
        }
        for (int i = len - 1; i > dot; --i) {
            if (text_buffer[i] == '0') {
                text_buffer[i] = '\0';
                if (i == dot + 1) {
                    text_buffer[dot] = '\0';
                }
            }
        }
    }

    void Reset() {
        memset(text_buffer, 0, sizeof(text_buffer));
        if (value) {
            *value = 0;
        }
    }

    void Draw() {
        if (GuiValueBoxFloat(box, text.c_str(), text_buffer, value, editmode)) {
            editmode = !editmode;
        }
        
        // Uncomment if want to modify *value not only from within input box
        // if (!editmode) {
        //     UpdateTextBuffer();
        // }
    }
};

struct GUI_Toggle {
    Rectangle box;
    std::string text_inactive;
    std::string text_active;
    bool active = false;

    GUI_Toggle() = default;

    GUI_Toggle(Rectangle box, std::string text_inactive, std::string text_active) :
        box(box), text_inactive(std::move(text_inactive)), text_active(std::move(text_active))
    {}

    void Draw() {
        GuiToggle(box, active ? text_active.c_str() : text_inactive.c_str(), &active);
    }
};