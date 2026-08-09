#pragma once
#include "utils/NonCopyable.h"
#include <string_view>
#include <raylib.h>

namespace Roguelike {

class Window : public NonCopyable {
public:
    Window(int width, int height, std::string_view title, int targetFPS = 144);
    ~Window();

    void ToggleFullscreen();
    [[nodiscard]] bool ShouldClose() const noexcept;
    [[nodiscard]] int GetWidth() const noexcept { return m_Width; }
    [[nodiscard]] int GetHeight() const noexcept { return m_Height; }
    [[nodiscard]] Vector2 GetDimensions() const noexcept { return Vector2{ static_cast<float>(m_Width), static_cast<float>(m_Height) }; }

private:
    int m_Width;
    int m_Height;
    bool m_IsFullscreen{false};
};

} // namespace Roguelike
