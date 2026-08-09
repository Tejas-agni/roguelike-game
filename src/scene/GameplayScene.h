#pragma once
#include "scene/Scene.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "entities/Projectile.h"
#include "entities/Gem.h"
#include "entities/Weapon.h"
#include "physics/SpatialHashGrid.h"
#include "utils/Timer.h"
#include <vector>
#include <memory>
#include <string>

namespace Roguelike {

class Game;

struct UpgradeOption {
    std::string title;
    std::string description;
    bool isWeapon;
    WeaponType weaponType;
    std::string passiveKey;
};

class GameplayScene : public Scene {
public:
    explicit GameplayScene(Game& game);
    ~GameplayScene() override = default;

    void OnEnter() override;
    void OnExit() override;

    void FixedUpdate(float fixedDeltaTime) override;
    void Update(float deltaTime) override;
    void RenderPixelPass() override;
    void RenderUIPass() override;

private:
    void SpawnEnemyWave(float deltaTime);
    void ResolveCollisions();
    void GenerateUpgradeOptions();
    void SelectUpgrade(int optionIndex);
    void RenderDarkFantasyArena();

    Game& m_Game;
    Player m_Player;

    std::vector<Enemy> m_Enemies;
    std::vector<Projectile> m_Projectiles;
    std::vector<Gem> m_Gems;

    SpatialHashGrid m_SpatialGrid{32.0f};

    Timer m_EnemySpawnTimer{0.8f, true};
    float m_SurvivalTime{0.0f};
    int m_KillsCount{0};
    uint32_t m_NextEntityID{1};

    // Wave / Difficulty scaling
    int m_CurrentWave{1};
    float m_WaveTimer{0.0f};
    bool m_BossSpawnedThisWave{false};

    // Level-up Modal State
    bool m_IsLevelUpActive{false};
    std::vector<UpgradeOption> m_CurrentOptions;
};

} // namespace Roguelike
