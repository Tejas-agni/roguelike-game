#pragma once
#include "utils/NonCopyable.h"
#include "core/Window.h"
#include "graphics/PixelRenderer.h"
#include "input/InputManager.h"
#include "resources/ResourceManager.h"
#include "audio/AudioManager.h"
#include "scene/SceneManager.h"
#include <memory>

namespace Roguelike {

class Game : public NonCopyable {
public:
    Game();
    ~Game();

    void Run();

    [[nodiscard]] Window& GetWindow() noexcept { return *m_Window; }
    [[nodiscard]] PixelRenderer& GetRenderer() noexcept { return *m_Renderer; }
    [[nodiscard]] InputManager& GetInput() noexcept { return *m_Input; }
    [[nodiscard]] ResourceManager& GetResources() noexcept { return *m_Resources; }
    [[nodiscard]] AudioManager& GetAudio() noexcept { return *m_Audio; }
    [[nodiscard]] SceneManager& GetScenes() noexcept { return *m_SceneManager; }

    [[nodiscard]] static Game& GetInstance() noexcept { return *s_Instance; }

private:
    void ProcessInput();
    void Update(float deltaTime);
    void Render();

    static inline Game* s_Instance{nullptr};

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<PixelRenderer> m_Renderer;
    std::unique_ptr<InputManager> m_Input;
    std::unique_ptr<ResourceManager> m_Resources;
    std::unique_ptr<AudioManager> m_Audio;
    std::unique_ptr<SceneManager> m_SceneManager;

    bool m_ShowDebugOverlay{false};
    float m_FixedAccumulator{0.0f};
    static constexpr float FIXED_TIMESTEP{ 1.0f / 60.0f }; // 60Hz fixed logic tick
};

} // namespace Roguelike
