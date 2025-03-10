#pragma once

#include <raylib.h>


struct Scene {
    virtual void Draw()           = 0;
    virtual void Update(float dt) = 0;
    virtual bool IsSwitchable() { return true; }
    virtual ~Scene() = default;
};

// TODO: do smth with this defines
//       they are kind of... odd
#define PANEL_X  (static_cast<float>(GetScreenWidth()) - 400) // static_cast bruh...
#define PANEL_Y  40
#define PANEL_W  400
#define PANEL_H  (static_cast<float>(GetScreenHeight()) - 40) // static_cast bruh...

#define PANEL (Rectangle { PANEL_X - 10, PANEL_Y - 10, PANEL_W - 10, PANEL_H - 10 })

#define BUTTON_W 80
#define BUTTON_H 30
