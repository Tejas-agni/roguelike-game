#include "entities/Weapon.h"
#include "entities/Enemy.h"
#include "entities/Projectile.h"
#include <raymath.h>
#include <algorithm>

namespace Roguelike {

Weapon::Weapon(WeaponType type)
    : m_Type(type)
{
    switch (m_Type) {
    case WeaponType::SwordSlash:
        m_Stats = { "Sword Slash", 1, 5, 25.0f, 0.8f, 50.0f, 1, 0.0f, 1.0f, false };
        break;
    case WeaponType::MagicWand:
        m_Stats = { "Magic Wand", 1, 5, 18.0f, 0.5f, 180.0f, 1, 200.0f, 1.0f, false };
        break;
    case WeaponType::AxeOrbit:
        m_Stats = { "Axe Orbit", 1, 5, 30.0f, 1.2f, 40.0f, 2, 140.0f, 1.0f, false };
        break;
    case WeaponType::WhipLash:
        m_Stats = { "Whip Lash", 1, 5, 22.0f, 0.9f, 65.0f, 1, 0.0f, 1.0f, false };
        break;
    case WeaponType::HolyBible:
        m_Stats = { "Holy Bible", 1, 5, 12.0f, 0.01f, 35.0f, 2, 90.0f, 1.0f, false }; // continuous orbit
        break;
    case WeaponType::GarlicAura:
        m_Stats = { "Garlic Aura", 1, 5, 8.0f, 0.3f, 30.0f, 1, 0.0f, 1.0f, false };
        break;
    case WeaponType::ArcaneCascade:
        m_Stats = { "Arcane Cascade", 1, 1, 45.0f, 0.2f, 250.0f, 3, 260.0f, 1.5f, true };
        break;
    case WeaponType::DeathScythe:
        m_Stats = { "Death Scythe", 1, 1, 70.0f, 0.5f, 80.0f, 2, 0.0f, 1.8f, true };
        break;
    }
}

void Weapon::Update(float deltaTime, Vector2 playerPos, const std::vector<Enemy>& enemies, std::vector<Projectile>& outProjectiles) {
    m_CooldownTimer -= deltaTime;
    m_OrbitAngle += m_Stats.speed * 0.05f * deltaTime;

    if (m_CooldownTimer <= 0.0f) {
        Fire(playerPos, enemies, outProjectiles);
        m_CooldownTimer = m_Stats.cooldown;
    }
}

void Weapon::Fire(Vector2 playerPos, const std::vector<Enemy>& enemies, std::vector<Projectile>& outProjectiles) {
    // Find nearest enemy for targeted weapons
    const Enemy* nearest = nullptr;
    float minDistSq = m_Stats.range * m_Stats.range;

    for (const auto& enemy : enemies) {
        if (!enemy.IsAlive()) continue;
        float d2 = Vector2DistanceSqr(playerPos, enemy.GetPosition());
        if (d2 < minDistSq) {
            minDistSq = d2;
            nearest = &enemy;
        }
    }

    Vector2 defaultDir{ 1.0f, 0.0f };
    if (nearest) {
        defaultDir = Vector2Normalize(Vector2Subtract(nearest->GetPosition(), playerPos));
    }

    switch (m_Type) {
    case WeaponType::MagicWand:
    case WeaponType::ArcaneCascade:
        for (int i = 0; i < m_Stats.projectileCount; ++i) {
            float spread = (i - (m_Stats.projectileCount - 1) / 2.0f) * 0.2f;
            Vector2 dir = Vector2Rotate(defaultDir, spread);
            outProjectiles.push_back(Projectile(playerPos, dir, m_Stats.speed, m_Stats.damage, 3.0f, Color{ 100, 200, 255, 255 }));
        }
        break;

    case WeaponType::SwordSlash:
    case WeaponType::DeathScythe:
        for (int i = 0; i < m_Stats.projectileCount; ++i) {
            Vector2 dir = (i % 2 == 0) ? defaultDir : Vector2Scale(defaultDir, -1.0f);
            outProjectiles.push_back(Projectile(playerPos, dir, 0.0f, m_Stats.damage, 0.2f, Color{ 255, 230, 100, 255 }));
        }
        break;

    case WeaponType::WhipLash:
        outProjectiles.push_back(Projectile(playerPos, defaultDir, 300.0f, m_Stats.damage, 0.15f, Color{ 220, 80, 255, 255 }));
        break;

    case WeaponType::GarlicAura:
        // Garlic aura pulses around player
        for (int i = 0; i < 8; ++i) {
            float angle = i * (PI / 4.0f);
            Vector2 dir{ cosf(angle), sinf(angle) };
            outProjectiles.push_back(Projectile(playerPos, dir, 40.0f, m_Stats.damage, 0.15f, Color{ 180, 255, 140, 180 }));
        }
        break;

    default:
        break;
    }
}

void Weapon::Render(Vector2 playerPos) {
    if (m_Type == WeaponType::HolyBible || m_Type == WeaponType::AxeOrbit) {
        // Render orbiting visual elements around player
        for (int i = 0; i < m_Stats.projectileCount; ++i) {
            float angle = m_OrbitAngle + i * (2.0f * PI / m_Stats.projectileCount);
            float orbitRadius = m_Stats.range * m_Stats.areaScale;
            Vector2 pos{ playerPos.x + cosf(angle) * orbitRadius, playerPos.y + sinf(angle) * orbitRadius };

            Color col = (m_Type == WeaponType::HolyBible) ? Color{ 240, 240, 120, 230 } : Color{ 200, 200, 220, 230 };
            DrawCircleV(pos, 5.0f * m_Stats.areaScale, col);
            DrawCircleLinesV(pos, 6.0f * m_Stats.areaScale, WHITE);
        }
    }
    else if (m_Type == WeaponType::GarlicAura) {
        // Soft aura ring around player
        DrawCircleLinesV(playerPos, m_Stats.range * m_Stats.areaScale, Color{ 150, 255, 150, 100 });
        DrawCircleV(playerPos, m_Stats.range * m_Stats.areaScale, Color{ 150, 255, 150, 30 });
    }
}

void Weapon::LevelUp() {
    if (IsMaxLevel()) return;
    m_Stats.level++;
    m_Stats.damage *= 1.25f;
    m_Stats.cooldown = std::max(0.1f, m_Stats.cooldown * 0.9f);
    if (m_Stats.level % 2 == 0) {
        m_Stats.projectileCount++;
    }
}

void Weapon::Evolve(WeaponType evolvedType) {
    m_Type = evolvedType;
    m_Stats.isEvolved = true;
    m_Stats.level = 1;
    m_Stats.damage *= 2.0f;
    m_Stats.cooldown *= 0.6f;
    m_Stats.projectileCount += 2;
}

} // namespace Roguelike
