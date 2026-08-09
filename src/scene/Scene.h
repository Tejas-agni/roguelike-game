#pragma once
#include <raylib.h>

namespace Roguelike {

class Game; // Forward declaration

class Scene {
public:
    virtual ~Scene() = default;

    virtual void OnEnter() {}
    virtual void OnExit() {}

    virtual void FixedUpdate(float fixedDeltaTime) = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void RenderPixelPass() = 0; // Offscreen pixel art pass
    virtual void RenderUIPass() = 0;    // Screen-space UI pass
};

} // namespace Roguelike
