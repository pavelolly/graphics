#include <raylib.h>

#include <cstring>
#include <algorithm>

#include "gui.hpp"

namespace GUI {

InputBox::InputBox(Rectangle box, int *value, std::string text) : box(box), value_ptr(value), text(std::move(text)) {
    if (value) {
        UpdateTextBuffer();
    }
}

InputBox::InputBox(Rectangle box, float *value, std::string text) : box(box), value_ptr(value), text(std::move(text)) {
    if (value) {
        UpdateTextBuffer();
    }
}

void InputBox::UpdateTextBuffer() {
    if (value_ptr.index() == std::variant_npos) {
        return;
    }

    if (std::holds_alternative<int *>(value_ptr)) {
        snprintf(text_buffer, RAYGUI_VALUEBOX_MAX_CHARS + 1, "%d", *std::get<int *>(value_ptr));
        return;
    }

    int len = snprintf(text_buffer, RAYGUI_VALUEBOX_MAX_CHARS + 1, "%f", *std::get<float *>(value_ptr));

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

void InputBox::Reset() {
    text_buffer[0] = '0';
    std::fill(text_buffer + 1, text_buffer + RAYGUI_VALUEBOX_MAX_CHARS + 1, '\0');

    if (std::holds_alternative<int *>(value_ptr)) {
        *std::get<int *>(value_ptr) = 0;
    } else {
        *std::get<float *>(value_ptr) = 0.f;
    }
}

void InputBox::Draw() {
    if (std::holds_alternative<int *>(value_ptr)) {

        if (GuiValueBox(box, text.c_str(), std::get<int *>(value_ptr), 1, 10, editmode)) {
            editmode = !editmode;
        }

    } else {

        if (GuiValueBoxFloat(box, text.c_str(), text_buffer, std::get<float *>(value_ptr), editmode)) {
            editmode = !editmode;
        }

    }
    // Uncomment if want to modify *value not only from within input box
    // if (!editmode) {
    //     UpdateTextBuffer();
    // }
}

void InputBoxPanel::Add(float *value, std::string text) {
    size_t nboxes = input_boxes.size();
    Rectangle input_box = { panel.x + panel.width / 2,
                            DEFAULT_MARGIN + panel.y + nboxes * (DEFAULT_BOX_HEIGHT + DEFAULT_BOX_PADDING),
                            DEFAULT_BOX_WIDTH,
                            DEFAULT_BOX_HEIGHT };
    input_boxes.emplace_back(input_box, value, std::move(text));
}

void InputBoxPanel::Add(int *value, std::string text) {
    size_t nboxes = input_boxes.size();
    Rectangle input_box = { panel.x + panel.width / 2,
                            DEFAULT_MARGIN + panel.y + nboxes * (DEFAULT_BOX_HEIGHT + DEFAULT_BOX_PADDING),
                            DEFAULT_BOX_WIDTH,
                            DEFAULT_BOX_HEIGHT };
    input_boxes.emplace_back(input_box, value, std::move(text));
}

void InputBoxPanel::Draw() {
    // fill with background color to hide scene behind
    DrawRectangleRec(panel, GetColor(0x181818ff));
    GuiGroupBox(panel, "Parameters");

    for (auto &input_box : input_boxes) {
        input_box.Draw();
    }
}

void InputBoxPanel::Reset() {
    for (auto &input_box : input_boxes) {
        input_box.Reset();
    }
}

} // namespace GUI