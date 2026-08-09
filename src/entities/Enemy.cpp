#include "entities/Enemy.h"
#include <raymath.h>
#include <algorithm>

namespace Roguelike {

Enemy::Enemy(uint32_t id, EnemyType type, Vector2 spawnPosition)
    : m_ID(id)
    , m_Type(type)
    , m_Position(spawnPosition)
{
    switch (m_Type) {
    case EnemyType::Bat:
        m_Name = "Cave Bat";
        m_Speed = 85.0f;
        m_Radius = 5.0f;
        m_Health = 12.0f;
        m_Damage = 6.0f;
        m_XPReward = 10;
        m_BaseColor = Color{ 140, 70, 180, 255 }; // Purple bat
        break;

    case EnemyType::Zombie:
        m_Name = "Rotting Zombie";
        m_Speed = 50.0f;
        m_Radius = 7.0f;
        m_Health = 30.0f;
        m_Damage = 12.0f;
        m_XPReward = 20;
        m_BaseColor = Color{ 70, 160, 90, 255 }; // Decay green
        break;

    case EnemyType::Skeleton:
        m_Name = "Skeleton Warrior";
        m_Speed = 70.0f;
        m_Radius = 6.0f;
        m_Health = 22.0f;
        m_Damage = 10.0f;
        m_XPReward = 18;
        m_BaseColor = Color{ 220, 220, 210, 255 }; // Bone white
        break;

    case EnemyType::ArmoredKnight:
        m_Name = "Dread Knight";
        m_Speed = 40.0f;
        m_Radius = 10.0f;
        m_Health = 120.0f;
        m_Damage = 25.0f;
        m_XPReward = 60;
        m_BaseColor = Color{ 100, 110, 130, 255 }; // Dark steel
        break;

    case EnemyType::NecromancerBoss:
        m_Name = "Lich Lord";
        m_Speed = 30.0f;
        m_Radius = 16.0f;
        m_Health = 600.0f;
        m_Damage = 40.0f;
        m_XPReward = 300;
        m_BaseColor = Color{ 230, 40, 40, 255 }; // Crimson Red Lich
        break;
    }

    m_MaxHealth = m_Health;
}

void Enemy::Update(float deltaTime, Vector2 playerPosition) {
    if (!IsAlive()) return;

    if (m_FlashTimer > 0.0f) {
        m_FlashTimer -= deltaTime;
    }

    // Steering behavior: Move directly towards player position
    Vector2 direction = Vector2Subtract(playerPosition, m_Position);
    if (Vector2LengthSqr(direction) > 0.001f) {
        direction = Vector2Normalize(direction);
    }

    m_Velocity = Vector2Scale(direction, m_Speed);
    m_Position = Vector2Add(m_Position, Vector2Scale(m_Velocity, deltaTime));
}

void Enemy::Render() {
    if (!IsAlive()) return;

    Color renderColor = m_BaseColor;
    if (m_FlashTimer > 0.0f) {
        renderColor = WHITE;
    }

    // Body
    DrawCircleV(m_Position, m_Radius, renderColor);
    DrawCircleLinesV(m_Position, m_Radius + 0.5f, Color{ 10, 10, 15, 255 });

    // Boss health bar above head
    if (m_Type == EnemyType::NecromancerBoss) {
        const float hpPct = m_Health / m_MaxHealth;
        Vector2 barPos{ m_Position.x - 16.0f, m_Position.y - m_Radius - 8.0f };
        DrawRectangleV(barPos, { 32.0f, 3.0f }, Color{ 30, 30, 40, 200 });
        DrawRectangleV(barPos, { 32.0f * hpPct, 3.0f }, RED);
    }
}

void Enemy::TakeDamage(float damage) {
    if (!IsAlive()) return;
    m_Health -= damage;
    m_FlashTimer = 0.1f;
}

} // namespace Roguelike
