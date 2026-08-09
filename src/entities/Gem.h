#pragma once
#include <raylib.h>

namespace Roguelike {

class Gem {
public:
    Gem(Vector2 position, int xpValue);

    void Update(float deltaTime, Vector2 playerPosition, float magnetRadius);
    void Render();

    [[nodiscard]] Vector2 GetPosition() const noexcept { return m_Position; }
    [[nodiscard]] float GetRadius() const noexcept { return m_Radius; }
    [[nodiscard]] int GetXPValue() const noexcept { return m_XPValue; }
    [[nodiscard]] bool IsActive() const noexcept { return m_Active; }
    void Collect() { m_Active = false; }

private:
    Vector2 m_Position;
    int m_XPValue;
    float m_Radius{3.0f};
    bool m_Active{true};
    float m_PullSpeed{0.0f};
};

} // namespace Roguelike
