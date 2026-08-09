#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include <memory>

namespace Roguelike {

class Enemy;
class Projectile;

enum class WeaponType {
    SwordSlash,
    MagicWand,
    AxeOrbit,
    WhipLash,
    HolyBible,
    GarlicAura,

    // Evolved
    ArcaneCascade, // Evolved MagicWand
    DeathScythe     // Evolved Sword
};

struct WeaponStats {
    std::string name;
    int level{1};
    int maxLevel{5};
    float damage{15.0f};
    float cooldown{1.0f};
    float range{120.0f};
    int projectileCount{1};
    float speed{180.0f};
    float areaScale{1.0f};
    bool isEvolved{false};
};

class Weapon {
public:
    Weapon(WeaponType type);
    virtual ~Weapon() = default;

    void Update(float deltaTime, Vector2 playerPos, const std::vector<Enemy>& enemies, std::vector<Projectile>& outProjectiles);
    void Render(Vector2 playerPos);

    void LevelUp();
    [[nodiscard]] const WeaponStats& GetStats() const noexcept { return m_Stats; }
    [[nodiscard]] WeaponType GetType() const noexcept { return m_Type; }
    [[nodiscard]] bool IsMaxLevel() const noexcept { return m_Stats.level >= m_Stats.maxLevel; }

    void Evolve(WeaponType evolvedType);

private:
    void Fire(Vector2 playerPos, const std::vector<Enemy>& enemies, std::vector<Projectile>& outProjectiles);

    WeaponType m_Type;
    WeaponStats m_Stats;
    float m_CooldownTimer{0.0f};
    float m_OrbitAngle{0.0f};
};

} // namespace Roguelike
