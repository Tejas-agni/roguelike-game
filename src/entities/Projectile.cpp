#include "entities/Projectile.h"
#include <raymath.h>

namespace Roguelike {

Projectile::Projectile(Vector2 position, Vector2 direction, float speed, float damage, float lifetime, Color color, int pierceCount)
    : m_Position(position)
    , m_Direction(Vector2Normalize(direction))
    , m_Speed(speed)
    , m_Damage(damage)
    , m_Lifetime(lifetime)
    , m_Color(color)
    , m_PierceCount(pierceCount)
{
}

void Projectile::Update(float deltaTime) {
    if (!m_Active) return;

    m_Lifetime -= deltaTime;
    if (m_Lifetime <= 0.0f) {
        m_Active = false;
        return;
    }

    m_Position = Vector2Add(m_Position, Vector2Scale(m_Direction, m_Speed * deltaTime));

    // Deactivate if out of arena screen bounds
    if (m_Position.x < -20.0f || m_Position.x > 500.0f || m_Position.y < -20.0f || m_Position.y > 290.0f) {
        m_Active = false;
    }
}

void Projectile::Render() {
    if (!m_Active) return;

    DrawCircleV(m_Position, m_Radius, m_Color);
    DrawCircleLinesV(m_Position, m_Radius + 0.5f, Color{ 255, 255, 255, 180 });
}

} // namespace Roguelike
