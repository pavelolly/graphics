#include <cstring>

#include "gui.hpp"

GUI_InputBox::GUI_InputBox(Rectangle box, float *value, std::string text) : box(box), value(value), text(std::move(text)) {
    if (value) {
        UpdateTextBuffer();
    }
}

void GUI_InputBox::UpdateTextBuffer() {
    int len = snprintf(text_buffer, RAYGUI_VALUEBOX_MAX_CHARS + 1, "%f", *value);

    /* truncate insignificant zeros */
    int dot = -1;
    for (int i = 0; i < len; ++i) {
        if (text_buffer[i] == '.') {
            dot = i;
            break;
        }
    }
    if (dot == -1) {
        return;
    }

    for (int i = len - 1; i > dot; --i) {
        if (text_buffer[i] != '0') {
            break;
        }

        text_buffer[i] = '\0';
        if (i == dot + 1) {
            text_buffer[dot] = '\0';
        }
    }
}

void GUI_InputBox::Reset() {
    text_buffer[0] = '0';
    memset(text_buffer + 1, '\0', RAYGUI_VALUEBOX_MAX_CHARS);

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