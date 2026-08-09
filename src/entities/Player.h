#pragma once
#include <raylib.h>
#include "utils/Timer.h"
#include "entities/Weapon.h"
#include <vector>

namespace Roguelike {

struct PassiveUpgrades {
    float moveSpeedMultiplier{1.0f};
    float maxHealthMultiplier{1.0f};
    float damageMultiplier{1.0f};
    float cooldownMultiplier{1.0f};
    float magnetMultiplier{1.0f};
    int shieldCount{0};
};

class Player {
public:
    Player(Vector2 startPosition);

    void Update(float deltaTime, Vector2 moveInput, const std::vector<Enemy>& enemies, std::vector<Projectile>& outProjectiles);
    void Render();

    void TakeDamage(float damage);
    void Heal(float amount);
    bool AddXP(int amount); // returns true if leveled up!

    void AddWeapon(WeaponType type);
    void UpgradePassive(const std::string& passiveName);

    [[nodiscard]] Vector2 GetPosition() const noexcept { return m_Position; }
    [[nodiscard]] float GetRadius() const noexcept { return m_Radius; }
    [[nodiscard]] float GetHealth() const noexcept { return m_Health; }
    [[nodiscard]] float GetMaxHealth() const noexcept { return m_MaxHealth * m_Passives.maxHealthMultiplier; }
    [[nodiscard]] int GetLevel() const noexcept { return m_Level; }
    [[nodiscard]] int GetXP() const noexcept { return m_XP; }
    [[nodiscard]] int GetNextLevelXP() const noexcept { return m_NextLevelXP; }
    [[nodiscard]] bool IsAlive() const noexcept { return m_Health > 0.0f; }
    [[nodiscard]] bool IsInvincible() const noexcept { return m_InvincibilityTimer.IsRunning(); }
    [[nodiscard]] float GetMagnetRadius() const noexcept { return m_MagnetRadius * m_Passives.magnetMultiplier; }

    [[nodiscard]] const std::vector<Weapon>& GetWeapons() const noexcept { return m_Weapons; }
    std::vector<Weapon>& GetWeapons() noexcept { return m_Weapons; }
    [[nodiscard]] const PassiveUpgrades& GetPassives() const noexcept { return m_Passives; }

private:
    Vector2 m_Position;
    Vector2 m_Velocity{0.0f, 0.0f};
    float m_BaseSpeed{120.0f};
    float m_Radius{8.0f};

    float m_Health{100.0f};
    float m_MaxHealth{100.0f};

    int m_Level{1};
    int m_XP{0};
    int m_NextLevelXP{100};
    float m_MagnetRadius{60.0f};

    Timer m_InvincibilityTimer{0.5f};
    float m_FlashTimer{0.0f};

    std::vector<Weapon> m_Weapons;
    PassiveUpgrades m_Passives;
};

} // namespace Roguelike
