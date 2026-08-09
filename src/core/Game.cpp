#include "core/Game.h"
#include "core/Logger.h"
#include "scene/GameplayScene.h"
#include <format>
#include <algorithm>
#include <raylib.h>

namespace Roguelike {

// Initial Title Scene transitioning to GameplayScene
class InitialTitleScene : public Scene {
public:
    explicit InitialTitleScene(Game& game) : m_Game(game) {}

    void FixedUpdate(float fixedDeltaTime) override {
        (void)fixedDeltaTime;
    }

    void Update(float deltaTime) override {
        (void)deltaTime;
        // Press SPACE / Attack to transition into combat
        if (m_Game.GetInput().IsActionPressed(InputAction::Attack)) {
            Logger::Info("Starting Roguelike Survival Game Run...");
            m_Game.GetScenes().ChangeScene(std::make_unique<GameplayScene>(m_Game));
        }
    }

    void RenderPixelPass() override {
        ClearBackground(Color{ 18, 18, 28, 255 });

        const char* title = "ROGUELIKE SURVIVAL";
        const char* subtitle = "Press SPACE / A Button to Begin";

        int titleWidth = MeasureText(title, 20);
        int subWidth = MeasureText(subtitle, 10);

        DrawText(title, (480 - titleWidth) / 2, 90, 20, Color{ 240, 200, 80, 255 });
        DrawText(subtitle, (480 - subWidth) / 2, 140, 10, Color{ 200, 200, 220, 255 });

        // Decorative pixel art frame accent
        DrawRectangleLinesEx(Rectangle{ 20.0f, 20.0f, 440.0f, 230.0f }, 2.0f, Color{ 80, 120, 160, 255 });
    }

    void RenderUIPass() override {
    }

private:
    Game& m_Game;
};

Game::Game() {
    s_Instance = this;

    m_Window = std::make_unique<Window>(1280, 720, "Roguelike Survival (C++20)", 144);
    m_Renderer = std::make_unique<PixelRenderer>(480, 270);
    m_Input = std::make_unique<InputManager>();
    m_Resources = std::make_unique<ResourceManager>();
    m_Audio = std::make_unique<AudioManager>(*m_Resources);
    m_SceneManager = std::make_unique<SceneManager>();

    // Push initial scene
    m_SceneManager->PushScene(std::make_unique<InitialTitleScene>(*this));
    m_SceneManager->ProcessPendingChanges();

    Logger::Info("Game engine initialized successfully.");
}

Game::~Game() {
    s_Instance = nullptr;
    Logger::Info("Game engine shut down.");
}

void Game::Run() {
    while (!m_Window->ShouldClose()) {
        float deltaTime = GetFrameTime();
        if (deltaTime > 0.25f) deltaTime = 0.25f; // Clamp spiral of death frame time drops

        ProcessInput();
        Update(deltaTime);
        Render();
    }
}

void Game::ProcessInput() {
    m_Input->Update();

    if (m_Input->IsActionPressed(InputAction::DebugToggle)) {
        m_ShowDebugOverlay = !m_ShowDebugOverlay;
        Logger::Info("Debug overlay toggled: {}", m_ShowDebugOverlay);
    }
}

void Game::Update(float deltaTime) {
    m_SceneManager->ProcessPendingChanges();

    // Fixed timestep accumulator for deterministic physics ticks
    m_FixedAccumulator += deltaTime;
    while (m_FixedAccumulator >= FIXED_TIMESTEP) {
        m_SceneManager->FixedUpdate(FIXED_TIMESTEP);
        m_FixedAccumulator -= FIXED_TIMESTEP;
    }

    m_SceneManager->Update(deltaTime);
    m_Audio->Update();
}

void Game::Render() {
    // 1. Offscreen 2D Pixel-Art Render Pass (480x270 virtual)
    m_Renderer->BeginPixelMode();
    m_SceneManager->RenderPixelPass();
    m_Renderer->EndPixelMode();

    // 2. Window upscale + screen-space UI in one BeginDrawing block
    {
        const int winW = m_Window->GetWidth();
        const int winH = m_Window->GetHeight();

        // Compute viewport for upscaling
        const float scaleX = static_cast<float>(winW) / static_cast<float>(m_Renderer->GetRenderWidth());
        const float scaleY = static_cast<float>(winH) / static_cast<float>(m_Renderer->GetRenderHeight());
        const float scale  = std::min(scaleX, scaleY);

        const float vpW = static_cast<float>(m_Renderer->GetRenderWidth())  * scale;
        const float vpH = static_cast<float>(m_Renderer->GetRenderHeight()) * scale;
        const float vpX = (static_cast<float>(winW) - vpW) * 0.5f;
        const float vpY = (static_cast<float>(winH) - vpH) * 0.5f;
        const Rectangle dest{ vpX, vpY, vpW, vpH };
        const Rectangle src{ 0.0f, 0.0f,
            static_cast<float>(m_Renderer->GetRenderWidth()),
            -static_cast<float>(m_Renderer->GetRenderHeight()) };

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(m_Renderer->GetRenderTexture().texture, src, dest, {0,0}, 0.0f, WHITE);

        // 3. Screen-space HUD pass (runs within active BeginDrawing)
        m_SceneManager->RenderUIPass();

        // 4. Debug overlay
        if (m_ShowDebugOverlay) {
            DrawFPS(10, 10);
            DrawText(TextFormat("Virtual: %dx%d", m_Renderer->GetRenderWidth(), m_Renderer->GetRenderHeight()), 10, 35, 16, GREEN);
            DrawText(TextFormat("Window:  %dx%d", winW, winH), 10, 55, 16, GREEN);
            DrawText(TextFormat("Scale:   %.2fx", scale), 10, 75, 16, GREEN);
        }

        EndDrawing();
    }
}

} // namespace Roguelike
