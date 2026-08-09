#include "entities/Gem.h"
#include <raymath.h>

namespace Roguelike {

Gem::Gem(Vector2 position, int xpValue)
    : m_Position(position)
    , m_XPValue(xpValue)
{
}

void Gem::Update(float deltaTime, Vector2 playerPosition, float magnetRadius) {
    if (!m_Active) return;

    float distSqr = Vector2DistanceSqr(m_Position, playerPosition);
    if (distSqr <= magnetRadius * magnetRadius) {
        m_PullSpeed += 300.0f * deltaTime; // Accelerate towards player
        Vector2 dir = Vector2Normalize(Vector2Subtract(playerPosition, m_Position));
        m_Position = Vector2Add(m_Position, Vector2Scale(dir, m_PullSpeed * deltaTime));
    }
}

void Gem::Render() {
    if (!m_Active) return;

    Color gemColor = Color{ 40, 240, 140, 255 }; // Emerald Green
    if (m_XPValue >= 40) {
        gemColor = Color{ 220, 80, 255, 255 }; // Purple High Value
    }

    DrawCircleV(m_Position, m_Radius, gemColor);
    DrawCircleV(m_Position, m_Radius * 0.4f, WHITE);
}

} // namespace Roguelike
