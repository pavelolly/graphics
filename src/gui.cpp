#include <raylib.h>

#include <cstring>
#include <algorithm>

#include "gui.hpp"

GUI_InputBox::GUI_InputBox(Rectangle box, float *value, std::string text) : box(box), value(value), text(std::move(text)) {
    if (value) {
        UpdateTextBuffer();
    }
}

void GUI_InputBox::UpdateTextBuffer() {
    int len = snprintf(text_buffer, RAYGUI_VALUEBOX_MAX_CHARS + 1, "%f", *value);

    /* truncate insignificant zeros */
    char *end = text_buffer + len;

    auto dot = std::find(text_buffer, end, '.');
    if (dot == end) {
        return;
    }

    for (auto it = end - 1; it > dot; --it) {
        if (*it != '0') {
            break;
        }

        *it = '\0';
        if (it == dot + 1) {
            *dot = '\0';
        }
    }
}

void GUI_InputBox::Reset() {
    text_buffer[0] = '0';
    std::fill(text_buffer + 1, text_buffer + RAYGUI_VALUEBOX_MAX_CHARS + 1, '\0');

    if (value) {
        *value = 0;
    }
}

void GUI_InputBox::Draw() {
    if (GuiValueBoxFloat(box, text.c_str(), text_buffer, value, editmode)) {
        editmode = !editmode;
    }

    // Uncomment if want to modify *value not only from within input box
    // if (!editmode) {
    //     UpdateTextBuffer();
    // }
}