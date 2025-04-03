#pragma once

#define RAYGUI_VALUEBOX_MAX_CHARS 10
#include <raygui.h>

#include <variant>
#include <vector>
#include <string>
#include <type_traits>
#include <limits>
#include <functional>

namespace GUI {

struct InputBox {
    Rectangle box;

    template <typename ValueType> requires std::is_same_v<ValueType, int> || std::is_same_v<ValueType, float>
    struct Value {
        ValueType *ptr;
        ValueType min;
        ValueType max;

        Value() = default;
        Value(ValueType *ptr) :
            ptr(ptr),
            min(std::numeric_limits<ValueType>::lowest()),
            max(std::numeric_limits<ValueType>::max())
        {}
        Value(ValueType *ptr, ValueType min, ValueType max) :
            ptr(ptr), min(min), max(max)
        {}
    };

    std::variant<Value<int>, Value<float>> value;

    std::string text;
    char text_buffer[RAYGUI_VALUEBOX_MAX_CHARS + 1] = {0};

    bool editmode = false;
    std::function<void(bool)> editmode_change_callback;

    InputBox() = default;
    InputBox(Rectangle box, int   *value, int min, int max, std::string text);
    InputBox(Rectangle box, float *value, std::string text);

    void UpdateTextBuffer();
    void Reset();
    void Draw();
};

struct InputBoxPanel {
    Rectangle panel;
    std::vector<GUI::InputBox> input_boxes;

    static const int DEFAULT_BOX_WIDTH   = 80;
    static const int DEFAULT_BOX_HEIGHT  = 30;
    static const int DEFAULT_BOX_PADDING = 10;
    static const int DEFAULT_MARGIN      = 20;

    InputBoxPanel() = default;
    InputBoxPanel(Rectangle box) : panel(box) {}

    void Add(float *value, std::string text="");
    void Add(int   *value, int min, int max, std::string text="");

    void Draw();
    void Reset();
};

struct Toggle {
    Rectangle box;
    std::string text_inactive;
    std::string text_active;
    bool active = false;

    Toggle() = default;
    Toggle(Rectangle box, std::string text_inactive, std::string text_active);
    
    void Draw();
};

} // namespace GUI
