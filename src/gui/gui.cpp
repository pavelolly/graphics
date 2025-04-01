#include <raylib.h>

#include <algorithm>

#include "colors.h"
#include "gui.hpp"

namespace GUI {

InputBox::InputBox(Rectangle box, int *value_ptr, int min, int max, std::string text) :
    box(box), value(InputBox::Value(value_ptr, min, max)), text(std::move(text))
{
    this->text += "  ";
    if (value_ptr) {
        UpdateTextBuffer();
    }
}

InputBox::InputBox(Rectangle box, float *value_ptr, std::string text) :
    box(box), value(value_ptr), text(std::move(text))
{
    this->text += "  ";
    if (value_ptr) {
        UpdateTextBuffer();
    }
}

void InputBox::UpdateTextBuffer() {
    if (std::holds_alternative<Value<int>>(value)) {
        std::snprintf(text_buffer, RAYGUI_VALUEBOX_MAX_CHARS + 1, "%d", *std::get<Value<int>>(value).ptr);
        return;
    }

    int len = std::snprintf(text_buffer, RAYGUI_VALUEBOX_MAX_CHARS + 1, "%f", *std::get<Value<float>>(value).ptr);
  
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


    if (std::holds_alternative<Value<int>>(value)) {
        auto ptr = std::get<Value<int>>(value).ptr;
        *ptr = 0;
    } else {
        auto ptr = std::get<Value<float>>(value).ptr;
        *ptr = 0.f;
    }
}

void InputBox::Draw() {
    if (std::holds_alternative<Value<int>>(value)) {
        
        auto &v = std::get<Value<int>>(value);
        if (GuiValueBox(box, text.c_str(), v.ptr, v.min, v.max, editmode)) {
            editmode = !editmode;
            if (editmode_change_callback) {
                editmode_change_callback(editmode);
            }
        }

    } else {

        auto &v = std::get<Value<float>>(value);
        if (GuiValueBoxFloat(box, text.c_str(), text_buffer, v.ptr, editmode)) {
            editmode = !editmode;
            if (editmode_change_callback) {
                editmode_change_callback(editmode);
            }
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

void InputBoxPanel::Add(int *value, int min, int max, std::string text) {
    size_t nboxes = input_boxes.size();
    Rectangle input_box = { panel.x + panel.width / 2,
                            DEFAULT_MARGIN + panel.y + nboxes * (DEFAULT_BOX_HEIGHT + DEFAULT_BOX_PADDING),
                            DEFAULT_BOX_WIDTH,
                            DEFAULT_BOX_HEIGHT };
    input_boxes.emplace_back(input_box, value, min, max, std::move(text));
}

void InputBoxPanel::Draw() {
    // fill with background color to hide scene behind
    DrawRectangleRec(panel, COLOR_BACKGROUND);
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