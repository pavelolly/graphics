#pragma once

struct Scene {
    virtual void Draw()           = 0;
    virtual void Update(float dt) = 0;
    virtual bool IsSwitchable() { return true; }
    virtual ~Scene() = default;
};
