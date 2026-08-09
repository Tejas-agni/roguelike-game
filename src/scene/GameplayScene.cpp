#include "scene/GameplayScene.h"
#include "core/Game.h"
#include "core/Logger.h"
#include "input/InputManager.h"
#include <raymath.h>
#include <algorithm>
#include <random>

namespace Roguelike {

GameplayScene::GameplayScene(Game& game)
    : m_Game(game)
    , m_Player(Vector2{ 240.0f, 135.0f })
{
}

void GameplayScene::OnEnter() {
    m_SurvivalTime = 0.0f;
    m_KillsCount = 0;
    m_NextEntityID = 1;
    m_Enemies.clear();
    m_Projectiles.clear();
    m_Gems.clear();
    m_IsLevelUpActive = false;
    m_CurrentWave = 1;
    m_WaveTimer = 0.0f;
    m_BossSpawnedThisWave = false;

    m_EnemySpawnTimer.Start();

    // Initialize audio cues procedurally
    m_Game.GetAudio().GetResourceManager().GenerateProceduralSound("PlayerHit", 150.0f, 0.15f, 0);   // Low square blip
    m_Game.GetAudio().GetResourceManager().GenerateProceduralSound("EnemyKilled", 80.0f, 0.12f, 2);   // Noise pop
    m_Game.GetAudio().GetResourceManager().GenerateProceduralSound("GemPickup", 880.0f, 0.1f, 1);    // High sine chime
    m_Game.GetAudio().GetResourceManager().GenerateProceduralSound("LevelUp", 440.0f, 0.4f, 1);      // Sine arpeggio
    m_Game.GetAudio().GetResourceManager().GenerateProceduralSound("MenuSelect", 600.0f, 0.08f, 0);  // Square blip

    Logger::Info("GameplayScene entered - Dark Fantasy Arena active!");
}

void GameplayScene::OnExit() {
    Logger::Info("GameplayScene exited.");
}

void GameplayScene::FixedUpdate(float fixedDeltaTime) {
    if (!m_Player.IsAlive() || m_IsLevelUpActive) return;

    // Fixed timestep spatial grid sync
    m_SpatialGrid.Clear();
    for (const auto& enemy : m_Enemies) {
        if (enemy.IsAlive()) {
            m_SpatialGrid.Insert(SpatialEntity{ enemy.GetID(), enemy.GetPosition(), enemy.GetRadius(), 1 });
        }
    }

    ResolveCollisions();
}

void GameplayScene::Update(float deltaTime) {
    // Handle level-up choice selection input
    if (m_IsLevelUpActive) {
        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) SelectUpgrade(0);
        else if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) SelectUpgrade(1);
        else if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) SelectUpgrade(2);
        return;
    }

    // Restart game if dead
    if (!m_Player.IsAlive()) {
        if (IsKeyPressed(KEY_R)) {
            OnEnter();
        }
        return;
    }

    m_SurvivalTime += deltaTime;
    m_WaveTimer += deltaTime;

    // Escalating wave difficulty every 5 minutes (300 sec)
    if (m_WaveTimer >= 300.0f) {
        m_WaveTimer = 0.0f;
        m_CurrentWave++;
        m_BossSpawnedThisWave = false;
        m_EnemySpawnTimer.SetDuration(std::max(0.2f, 0.8f - m_CurrentWave * 0.1f));
        Logger::Info("WAVE ADVANCED! Wave {}", m_CurrentWave);
    }

    // Spawn 5-min Boss
    if (!m_BossSpawnedThisWave && m_SurvivalTime > 10.0f && static_cast<int>(m_SurvivalTime) % 300 == 0) {
        m_Enemies.emplace_back(m_NextEntityID++, EnemyType::NecromancerBoss, Vector2{ 240.0f, 10.0f });
        m_BossSpawnedThisWave = true;
        Logger::Info("LICH LORD BOSS HAS SPAWNED!");
    }

    // Update Player & Weapons
    const Vector2 moveInput = m_Game.GetInput().GetMoveVector();
    m_Player.Update(deltaTime, moveInput, m_Enemies, m_Projectiles);

    // Update Enemies
    for (auto& enemy : m_Enemies) {
        if (enemy.IsAlive()) {
            enemy.Update(deltaTime, m_Player.GetPosition());
        }
    }

    // Update Projectiles
    for (auto& proj : m_Projectiles) {
        if (proj.IsActive()) {
            proj.Update(deltaTime);
        }
    }

    // Update Gems (attracted to player if within magnet radius)
    const Vector2 playerPos = m_Player.GetPosition();
    const float magR = m_Player.GetMagnetRadius();
    for (auto& gem : m_Gems) {
        if (gem.IsActive()) {
            gem.Update(deltaTime, playerPos, magR);

            // Gem pickup check
            if (Vector2DistanceSqr(gem.GetPosition(), playerPos) < (m_Player.GetRadius() + gem.GetRadius()) * (m_Player.GetRadius() + gem.GetRadius())) {
                gem.Collect();
                bool levelUp = m_Player.AddXP(gem.GetXPValue());
                m_Game.GetAudio().PlaySFX("GemPickup");
                if (levelUp) {
                    m_IsLevelUpActive = true;
                    GenerateUpgradeOptions();
                    m_Game.GetAudio().PlaySFX("LevelUp");
                }
            }
        }
    }

    // Spawn Enemies
    m_EnemySpawnTimer.Update(deltaTime);
    if (m_EnemySpawnTimer.IsFinished()) {
        SpawnEnemyWave(deltaTime);
        m_EnemySpawnTimer.Start();
    }

    // Clean up inactive projectiles and collected gems
    std::erase_if(m_Projectiles, [](const Projectile& p) { return !p.IsActive(); });
    std::erase_if(m_Gems, [](const Gem& g) { return !g.IsActive(); });
    std::erase_if(m_Enemies, [](const Enemy& e) { return !e.IsAlive(); });
}

void GameplayScene::SpawnEnemyWave(float deltaTime) {
    if (m_Enemies.size() >= 300) return; // Spatial performance cap

    // Random spawn position along the outer rectangle boundary
    Vector2 spawnPos{ 0.0f, 0.0f };
    int edge = GetRandomValue(0, 3);
    switch (edge) {
    case 0: spawnPos = { static_cast<float>(GetRandomValue(0, 480)), -10.0f }; break; // Top
    case 1: spawnPos = { static_cast<float>(GetRandomValue(0, 480)), 280.0f }; break; // Bottom
    case 2: spawnPos = { -10.0f, static_cast<float>(GetRandomValue(0, 270)) }; break; // Left
    case 3: spawnPos = { 490.0f, static_cast<float>(GetRandomValue(0, 270)) }; break; // Right
    }

    // Choose enemy type based on wave progression
    EnemyType type = EnemyType::Bat;
    int roll = GetRandomValue(1, 100);
    if (m_CurrentWave >= 3 && roll > 80) type = EnemyType::ArmoredKnight;
    else if (m_CurrentWave >= 2 && roll > 50) type = EnemyType::Skeleton;
    else if (roll > 40) type = EnemyType::Zombie;

    m_Enemies.emplace_back(m_NextEntityID++, type, spawnPos);
}

void GameplayScene::ResolveCollisions() {
    const Vector2 playerPos = m_Player.GetPosition();
    const float playerR = m_Player.GetRadius();

    // 1. Player vs Enemy collisions
    for (auto& enemy : m_Enemies) {
        if (!enemy.IsAlive()) continue;

        float minDist = playerR + enemy.GetRadius();
        if (Vector2DistanceSqr(playerPos, enemy.GetPosition()) < minDist * minDist) {
            m_Player.TakeDamage(enemy.GetDamage());
            m_Game.GetAudio().PlaySFX("PlayerHit");
        }
    }

    // 2. Projectiles vs Enemies
    for (auto& proj : m_Projectiles) {
        if (!proj.IsActive()) continue;

        // Query spatial hash grid for nearby entities
        auto candidateEntities = m_SpatialGrid.Query(proj.GetPosition(), proj.GetRadius() + 8.0f);
        for (const auto& entity : candidateEntities) {
            auto it = std::find_if(m_Enemies.begin(), m_Enemies.end(), [id = entity.id](const Enemy& e) { return e.GetID() == id; });
            if (it != m_Enemies.end() && it->IsAlive()) {
                float minDist = proj.GetRadius() + it->GetRadius();
                if (Vector2DistanceSqr(proj.GetPosition(), it->GetPosition()) < minDist * minDist) {
                    it->TakeDamage(proj.GetDamage());
                    proj.Deactivate();

                    if (!it->IsAlive()) {
                        m_KillsCount++;
                        m_Gems.emplace_back(it->GetPosition(), it->GetXPReward());
                        m_Game.GetAudio().PlaySFX("EnemyKilled");
                    }
                    break;
                }
            }
        }
    }

    // 3. Enemy vs Enemy separation (anti-stacking)
    for (size_t i = 0; i < m_Enemies.size(); ++i) {
        if (!m_Enemies[i].IsAlive()) continue;
        for (size_t j = i + 1; j < m_Enemies.size(); ++j) {
            if (!m_Enemies[j].IsAlive()) continue;

            Vector2 diff = Vector2Subtract(m_Enemies[i].GetPosition(), m_Enemies[j].GetPosition());
            float distSq = Vector2LengthSqr(diff);
            float minDist = m_Enemies[i].GetRadius() + m_Enemies[j].GetRadius();

            if (distSq < minDist * minDist && distSq > 0.0001f) {
                float dist = sqrtf(distSq);
                float overlap = 0.5f * (minDist - dist);
                Vector2 normal = Vector2Scale(diff, 1.0f / dist);

                m_Enemies[i].SetPosition(Vector2Add(m_Enemies[i].GetPosition(), Vector2Scale(normal, overlap)));
                m_Enemies[j].SetPosition(Vector2Subtract(m_Enemies[j].GetPosition(), Vector2Scale(normal, overlap)));
            }
        }
    }
}

void GameplayScene::GenerateUpgradeOptions() {
    m_CurrentOptions.clear();

    // Pool of available weapons & passives
    std::vector<UpgradeOption> pool = {
        { "Sword Slash", "Slash melee arc around player", true, WeaponType::SwordSlash, "" },
        { "Magic Wand", "Auto-fire arcane bolts at nearest foe", true, WeaponType::MagicWand, "" },
        { "Axe Orbit", "Orbit heavy axes in a wide lethal radius", true, WeaponType::AxeOrbit, "" },
        { "Whip Lash", "Lash out with a sweeping whip attack", true, WeaponType::WhipLash, "" },
        { "Holy Bible", "Orbit sacred scriptures protecting you", true, WeaponType::HolyBible, "" },
        { "Garlic Aura", "Pulsing toxic aura harming nearby foes", true, WeaponType::GarlicAura, "" },
        { "Swift Boots", "+15% Movement Speed", false, WeaponType::MagicWand, "MoveSpeed" },
        { "Vitality Ring", "+20% Max HP & restore health", false, WeaponType::MagicWand, "MaxHealth" },
        { "Magnetic Charm", "+40% Gem Magnet pickup radius", false, WeaponType::MagicWand, "Magnet" },
        { "Aegis Shield", "Absorb 1 incoming damage hit", false, WeaponType::MagicWand, "Shield" },
        { "Haste Tome", "-15% Weapon Cooldown time", false, WeaponType::MagicWand, "Cooldown" },
        { "Might Elixir", "+25% Overall Damage", false, WeaponType::MagicWand, "Damage" }
    };

    std::shuffle(pool.begin(), pool.end(), std::mt19937{ std::random_device{}() });
    for (size_t i = 0; i < 3 && i < pool.size(); ++i) {
        m_CurrentOptions.push_back(pool[i]);
    }
}

void GameplayScene::SelectUpgrade(int optionIndex) {
    if (optionIndex < 0 || optionIndex >= static_cast<int>(m_CurrentOptions.size())) return;

    const auto& choice = m_CurrentOptions[optionIndex];
    if (choice.isWeapon) {
        m_Player.AddWeapon(choice.weaponType);
    } else {
        m_Player.UpgradePassive(choice.passiveKey);
    }

    m_IsLevelUpActive = false;
    m_Game.GetAudio().PlaySFX("MenuSelect");
}

void GameplayScene::RenderDarkFantasyArena() {
    // 1. Dark stone floor tiles
    for (int x = 0; x < 480; x += 32) {
        for (int y = 0; y < 270; y += 32) {
            Color tileCol = ((x / 32 + y / 32) % 2 == 0) ? Color{ 22, 24, 32, 255 } : Color{ 18, 20, 26, 255 };
            DrawRectangle(x, y, 32, 32, tileCol);
            DrawRectangleLines(x, y, 32, 32, Color{ 14, 15, 20, 255 });
        }
    }

    // 2. Ruined Arena Border Walls
    DrawRectangleLinesEx(Rectangle{ 0, 0, 480, 270 }, 2.0f, Color{ 60, 65, 80, 255 });

    // 3. Flickering corner torches
    float torchFlicker = sinf(static_cast<float>(GetTime()) * 8.0f) * 2.0f;
    Vector2 torches[4] = { { 16, 16 }, { 464, 16 }, { 16, 254 }, { 464, 254 } };
    for (auto& t : torches) {
        DrawCircleV(t, 8.0f + torchFlicker, Color{ 255, 140, 40, 80 });
        DrawCircleV(t, 4.0f, Color{ 255, 200, 60, 255 });
    }
}

void GameplayScene::RenderPixelPass() {
    // Render offscreen 480x270 pixel art scene
    RenderDarkFantasyArena();

    // Render Gems
    for (auto& gem : m_Gems) {
        gem.Render();
    }

    // Render Enemies
    for (auto& enemy : m_Enemies) {
        enemy.Render();
    }

    // Render Projectiles
    for (auto& proj : m_Projectiles) {
        proj.Render();
    }

    // Render Player
    m_Player.Render();
}

void GameplayScene::RenderUIPass() {
    const Rectangle vp = m_Game.GetRenderer().GetViewportRectangle();
    const float scaleX = vp.width / static_cast<float>(m_Game.GetRenderer().GetRenderWidth());
    const float scaleY = vp.height / static_cast<float>(m_Game.GetRenderer().GetRenderHeight());

    auto toScreen = [&](float x, float y) -> Vector2 {
        return { vp.x + x * scaleX, vp.y + y * scaleY };
    };
    auto scW = [&](float w) { return w * scaleX; };
    auto scH = [&](float h) { return h * scaleY; };

    int fontSmall = static_cast<int>(10.0f * scaleX);
    int fontMed   = static_cast<int>(14.0f * scaleX);
    int fontLarge = static_cast<int>(22.0f * scaleX);

    // --- HUD Top Bar ---
    // HP Bar
    const float hpPercent = m_Player.GetHealth() / m_Player.GetMaxHealth();
    Vector2 hpPos = toScreen(10.0f, 8.0f);
    DrawRectangle(static_cast<int>(hpPos.x), static_cast<int>(hpPos.y), static_cast<int>(scW(120)), static_cast<int>(scH(10)), Color{ 30, 30, 40, 220 });
    DrawRectangle(static_cast<int>(hpPos.x), static_cast<int>(hpPos.y), static_cast<int>(scW(120) * hpPercent), static_cast<int>(scH(10)), Color{ 230, 50, 70, 255 });
    DrawRectangleLines(static_cast<int>(hpPos.x), static_cast<int>(hpPos.y), static_cast<int>(scW(120)), static_cast<int>(scH(10)), Color{ 255, 255, 255, 180 });

    // XP Bar
    const float xpPercent = static_cast<float>(m_Player.GetXP()) / static_cast<float>(m_Player.GetNextLevelXP());
    Vector2 xpPos = toScreen(10.0f, 20.0f);
    DrawRectangle(static_cast<int>(xpPos.x), static_cast<int>(xpPos.y), static_cast<int>(scW(120)), static_cast<int>(scH(5)), Color{ 30, 30, 40, 220 });
    DrawRectangle(static_cast<int>(xpPos.x), static_cast<int>(xpPos.y), static_cast<int>(scW(120) * xpPercent), static_cast<int>(scH(5)), Color{ 40, 220, 130, 255 });

    // Stats Info
    Vector2 lvlPos  = toScreen(138.0f, 7.0f);
    Vector2 wavePos = toScreen(200.0f, 7.0f);
    Vector2 timePos = toScreen(330.0f, 7.0f);
    Vector2 killPos = toScreen(330.0f, 20.0f);
    DrawText(TextFormat("LVL %d", m_Player.GetLevel()), static_cast<int>(lvlPos.x), static_cast<int>(lvlPos.y), fontSmall, Color{ 240, 220, 90, 255 });
    DrawText(TextFormat("WAVE %d", m_CurrentWave), static_cast<int>(wavePos.x), static_cast<int>(wavePos.y), fontSmall, Color{ 255, 150, 50, 255 });
    DrawText(TextFormat("TIME: %02d:%02d", static_cast<int>(m_SurvivalTime) / 60, static_cast<int>(m_SurvivalTime) % 60), static_cast<int>(timePos.x), static_cast<int>(timePos.y), fontSmall, WHITE);
    DrawText(TextFormat("KILLS: %d", m_KillsCount), static_cast<int>(killPos.x), static_cast<int>(killPos.y), fontSmall, Color{ 200, 200, 220, 255 });

    // --- Level-Up Upgrade Modal Overlay ---
    if (m_IsLevelUpActive) {
        // Dark translucent backdrop
        DrawRectangle(static_cast<int>(vp.x), static_cast<int>(vp.y), static_cast<int>(vp.width), static_cast<int>(vp.height), Color{ 10, 12, 18, 230 });

        // Title
        const char* titleText = "CHOOSE AN UPGRADE";
        int tw = MeasureText(titleText, fontLarge);
        Vector2 titlePos = toScreen(240.0f, 30.0f);
        DrawText(titleText, static_cast<int>(titlePos.x) - tw / 2, static_cast<int>(titlePos.y), fontLarge, Color{ 240, 220, 90, 255 });

        // 3 Cards
        for (size_t i = 0; i < m_CurrentOptions.size(); ++i) {
            float cardY = 75.0f + i * 55.0f;
            Vector2 cardPos = toScreen(60.0f, cardY);
            Vector2 cardSize = { scW(360.0f), scH(48.0f) };

            // Card background box
            DrawRectangle(static_cast<int>(cardPos.x), static_cast<int>(cardPos.y), static_cast<int>(cardSize.x), static_cast<int>(cardSize.y), Color{ 35, 40, 55, 240 });
            DrawRectangleLines(static_cast<int>(cardPos.x), static_cast<int>(cardPos.y), static_cast<int>(cardSize.x), static_cast<int>(cardSize.y), Color{ 100, 140, 200, 255 });

            // Key badge [1], [2], [3]
            DrawRectangle(static_cast<int>(cardPos.x) + 8, static_cast<int>(cardPos.y) + 8, static_cast<int>(scW(24.0f)), static_cast<int>(scH(32.0f)), Color{ 60, 80, 120, 255 });
            DrawText(TextFormat("%d", static_cast<int>(i + 1)), static_cast<int>(cardPos.x) + 14, static_cast<int>(cardPos.y) + 14, fontMed, WHITE);

            // Card text
            DrawText(m_CurrentOptions[i].title.c_str(), static_cast<int>(cardPos.x) + 40, static_cast<int>(cardPos.y) + 8, fontMed, Color{ 255, 230, 120, 255 });
            DrawText(m_CurrentOptions[i].description.c_str(), static_cast<int>(cardPos.x) + 40, static_cast<int>(cardPos.y) + 26, fontSmall, Color{ 200, 210, 230, 255 });
        }
    }

    // --- Game Over Overlay ---
    if (!m_Player.IsAlive()) {
        DrawRectangle(static_cast<int>(vp.x), static_cast<int>(vp.y), static_cast<int>(vp.width), static_cast<int>(vp.height), Color{ 10, 10, 15, 220 });
        const char* msg = "YOU DIED";
        int w = MeasureText(msg, fontLarge);
        Vector2 centre = toScreen(240.0f, 100.0f);
        DrawText(msg, static_cast<int>(centre.x) - w / 2, static_cast<int>(centre.y), fontLarge, RED);

        const char* sub = "Press R to Restart";
        int sw = MeasureText(sub, fontMed);
        Vector2 subPos = toScreen(240.0f, 145.0f);
        DrawText(sub, static_cast<int>(subPos.x) - sw / 2, static_cast<int>(subPos.y), fontMed, WHITE);
    }
}

} // namespace Roguelike
