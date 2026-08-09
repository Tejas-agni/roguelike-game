#pragma once
#include <raylib.h>
#include <cstdint>
#include <string>

namespace Roguelike {

enum class EnemyType {
    Bat,            // Swarm: fast, low hp, small radius
    Zombie,         // Runner: steady speed, medium hp
    Skeleton,       // Ranged/Runner: fast, moderate damage
    ArmoredKnight,  // Tank: slow, high hp, high damage
    NecromancerBoss // Boss: slow, massive hp, spawns minions
};

class Enemy {
public:
    Enemy(uint32_t id, EnemyType type, Vector2 spawnPosition);

    void Update(float deltaTime, Vector2 playerPosition);
    void Render();

    void TakeDamage(float damage);

    [[nodiscard]] uint32_t GetID() const noexcept { return m_ID; }
    [[nodiscard]] EnemyType GetType() const noexcept { return m_Type; }
    [[nodiscard]] Vector2 GetPosition() const noexcept { return m_Position; }
    [[nodiscard]] float GetRadius() const noexcept { return m_Radius; }
    [[nodiscard]] float GetHealth() const noexcept { return m_Health; }
    [[nodiscard]] float GetDamage() const noexcept { return m_Damage; }
    [[nodiscard]] int GetXPReward() const noexcept { return m_XPReward; }
    [[nodiscard]] bool IsAlive() const noexcept { return m_Health > 0.0f; }

    void SetPosition(Vector2 pos) { m_Position = pos; }

private:
    uint32_t m_ID;
    EnemyType m_Type;
    Vector2 m_Position;
    Vector2 m_Velocity{0.0f, 0.0f};

    float m_Speed{60.0f};
    float m_Radius{6.0f};
    float m_Health{20.0f};
    float m_MaxHealth{20.0f};
    float m_Damage{10.0f};
    int m_XPReward{15};

    float m_FlashTimer{0.0f};
    Color m_BaseColor{ 230, 60, 60, 255 };
    std::string m_Name;
};

} // namespace Roguelike
