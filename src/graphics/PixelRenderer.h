#pragma once
#include "utils/NonCopyable.h"
#include <raylib.h>

namespace Roguelike {

class PixelRenderer : public NonCopyable {
public:
    PixelRenderer(int renderWidth = 480, int renderHeight = 270);
    ~PixelRenderer();

    // Call before rendering 2D pixel art game world
    void BeginPixelMode();
    // Call after rendering 2D pixel art game world
    void EndPixelMode();

    // Access the underlying render texture (for Game::Render upscaling)
    [[nodiscard]] const RenderTexture2D& GetRenderTexture() const noexcept { return m_TargetBuffer; }

    [[nodiscard]] int GetRenderWidth() const noexcept { return m_RenderWidth; }
    [[nodiscard]] int GetRenderHeight() const noexcept { return m_RenderHeight; }
    [[nodiscard]] Vector2 GetVirtualResolution() const noexcept {
        return Vector2{ static_cast<float>(m_RenderWidth), static_cast<float>(m_RenderHeight) };
    }
    [[nodiscard]] Rectangle GetViewportRectangle() const noexcept { return m_ViewportRect; }

private:
    int m_RenderWidth;
    int m_RenderHeight;
    RenderTexture2D m_TargetBuffer{};
    Rectangle m_ViewportRect{};
    float m_ScaleFactor{1.0f};
};

} // namespace Roguelike
