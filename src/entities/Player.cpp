#include "entities/Player.h"
#include "core/Logger.h"
#include <raymath.h>
#include <algorithm>

namespace Roguelike {

Player::Player(Vector2 startPosition)
    : m_Position(startPosition)
{
    // Start with initial weapon: Magic Wand
    m_Weapons.emplace_back(WeaponType::MagicWand);
}

void Player::Update(float deltaTime, Vector2 moveInput, const std::vector<Enemy>& enemies, std::vector<Projectile>& outProjectiles) {
    if (!IsAlive()) return;

    m_InvincibilityTimer.Update(deltaTime);
    if (m_FlashTimer > 0.0f) m_FlashTimer -= deltaTime;

    // Movement math with passive speed multiplier
    const float currentSpeed = m_BaseSpeed * m_Passives.moveSpeedMultiplier;
    if (Vector2LengthSqr(moveInput) > 0.0f) {
        moveInput = Vector2Normalize(moveInput);
        m_Velocity = Vector2Scale(moveInput, currentSpeed);
    } else {
        m_Velocity = Vector2Lerp(m_Velocity, Vector2Zero(), 15.0f * deltaTime);
    }

    m_Position = Vector2Add(m_Position, Vector2Scale(m_Velocity, deltaTime));

    // Clamp inside arena bounds
    const float arenaPadding = m_Radius;
    m_Position.x = std::clamp(m_Position.x, arenaPadding, 480.0f - arenaPadding);
    m_Position.y = std::clamp(m_Position.y, arenaPadding, 270.0f - arenaPadding);

    // Update all weapons
    for (auto& weapon : m_Weapons) {
        weapon.Update(deltaTime, m_Position, enemies, outProjectiles);
    }
}

void Player::Render() {
    if (!IsAlive()) return;

    // Render weapons (orbiting effects, garlic aura, etc.)
    for (auto& weapon : m_Weapons) {
        weapon.Render(m_Position);
    }

    // Flash effect on hit
    Color playerColor = Color{ 80, 200, 240, 255 };
    if (IsInvincible() && static_cast<int>(GetTime() * 20.0) % 2 == 0) {
        playerColor = RED;
    }

    // Draw Dark Fantasy Knight / Hero (Pixel sprite simulation)
    // Body & Cloak
    DrawCircleV(m_Position, m_Radius, playerColor);
    DrawCircleLinesV(m_Position, m_Radius + 1.0f, Color{ 30, 40, 60, 255 });

    // Shield visual indicator
    if (m_Passives.shieldCount > 0) {
        DrawCircleLinesV(m_Position, m_Radius + 4.0f, Color{ 100, 220, 255, 200 });
    }
}

void Player::TakeDamage(float damage) {
    if (IsInvincible() || !IsAlive()) return;

    if (m_Passives.shieldCount > 0) {
        m_Passives.shieldCount--;
        m_InvincibilityTimer.Start();
        Logger::Info("Player shield absorbed hit! Shields remaining: {}", m_Passives.shieldCount);
        return;
    }

    m_Health = std::max(0.0f, m_Health - damage);
    m_InvincibilityTimer.Start();
    m_FlashTimer = 0.2f;
    Logger::Info("Player took {:.1f} damage! HP: {:.1f}/{:.1f}", damage, m_Health, GetMaxHealth());
}

void Player::Heal(float amount) {
    if (!IsAlive()) return;
    m_Health = std::min(GetMaxHealth(), m_Health + amount);
}

bool Player::AddXP(int amount) {
    m_XP += amount;
    bool leveledUp = false;

    while (m_XP >= m_NextLevelXP) {
        m_XP -= m_NextLevelXP;
        m_Level++;
        m_NextLevelXP = static_cast<int>(m_NextLevelXP * 1.3f);
        m_Health = GetMaxHealth(); // Full heal on level up!
        leveledUp = true;
        Logger::Info("Player LEVEL UP! Now Level {}", m_Level);
    }

    return leveledUp;
}

void Player::AddWeapon(WeaponType type) {
    // Check if weapon already owned; if so, level it up
    for (auto& w : m_Weapons) {
        if (w.GetType() == type) {
            w.LevelUp();
            return;
        }
    }
    m_Weapons.emplace_back(type);
}

void Player::UpgradePassive(const std::string& passiveName) {
    if (passiveName == "MoveSpeed") {
        m_Passives.moveSpeedMultiplier += 0.15f;
    } else if (passiveName == "MaxHealth") {
        m_Passives.maxHealthMultiplier += 0.2f;
        m_Health += 20.0f;
    } else if (passiveName == "Magnet") {
        m_Passives.magnetMultiplier += 0.4f;
    } else if (passiveName == "Shield") {
        m_Passives.shieldCount += 1;
    } else if (passiveName == "Cooldown") {
        m_Passives.cooldownMultiplier *= 0.85f;
    } else if (passiveName == "Damage") {
        m_Passives.damageMultiplier += 0.25f;
    }
}

} // namespace Roguelike
