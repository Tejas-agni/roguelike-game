#pragma once
#include <raylib.h>

namespace Roguelike {

class Projectile {
public:
    Projectile(Vector2 position, Vector2 direction, float speed, float damage, float lifetime = 3.0f, Color color = Color{ 100, 200, 255, 255 }, int pierceCount = 1);

    void Update(float deltaTime);
    void Render();

    [[nodiscard]] Vector2 GetPosition() const noexcept { return m_Position; }
    [[nodiscard]] float GetRadius() const noexcept { return m_Radius; }
    [[nodiscard]] float GetDamage() const noexcept { return m_Damage; }
    [[nodiscard]] bool IsActive() const noexcept { return m_Active; }
    [[nodiscard]] bool IsExpired() const noexcept { return m_Lifetime <= 0.0f || m_PierceCount <= 0; }

    void OnHit() {
        m_PierceCount--;
        if (m_PierceCount <= 0) m_Active = false;
    }

    void Deactivate() { m_Active = false; }

private:
    Vector2 m_Position;
    Vector2 m_Direction;
    float m_Speed;
    float m_Damage;
    float m_Radius{3.0f};
    float m_Lifetime{3.0f};
    Color m_Color{ 100, 200, 255, 255 };
    int m_PierceCount{1};
    bool m_Active{true};
};

} // namespace Roguelike
