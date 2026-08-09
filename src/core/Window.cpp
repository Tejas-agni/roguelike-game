#include "core/Window.h"
#include "core/Logger.h"

namespace Roguelike {

Window::Window(int width, int height, std::string_view title, int targetFPS)
    : m_Width(width)
    , m_Height(height)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(width, height, title.data());
    SetTargetFPS(targetFPS);

    Logger::Info("Window initialized: {}x{} @ {} FPS target", width, height, targetFPS);
}

Window::~Window() {
    CloseWindow();
    Logger::Info("Window closed cleanly.");
}

void Window::ToggleFullscreen() {
    m_IsFullscreen = !m_IsFullscreen;
    ::ToggleFullscreen();
    Logger::Info("Fullscreen state changed: {}", m_IsFullscreen);
}

bool Window::ShouldClose() const noexcept {
    return WindowShouldClose();
}

} // namespace Roguelike
