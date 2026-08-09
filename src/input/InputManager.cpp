#include "input/InputManager.h"
#include <raymath.h>

namespace Roguelike {

InputManager::InputManager() {
    // Default key & gamepad bindings
    m_Bindings.emplace(InputAction::MoveUp,      KeyBinding(KEY_W, GAMEPAD_BUTTON_LEFT_FACE_UP));
    m_Bindings.emplace(InputAction::MoveDown,    KeyBinding(KEY_S, GAMEPAD_BUTTON_LEFT_FACE_DOWN));
    m_Bindings.emplace(InputAction::MoveLeft,    KeyBinding(KEY_A, GAMEPAD_BUTTON_LEFT_FACE_LEFT));
    m_Bindings.emplace(InputAction::MoveRight,   KeyBinding(KEY_D, GAMEPAD_BUTTON_LEFT_FACE_RIGHT));
    m_Bindings.emplace(InputAction::Attack,      KeyBinding(KEY_SPACE, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));
    m_Bindings.emplace(InputAction::Dash,        KeyBinding(KEY_LEFT_SHIFT, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT));
    m_Bindings.emplace(InputAction::Interact,    KeyBinding(KEY_E, GAMEPAD_BUTTON_RIGHT_FACE_LEFT));
    m_Bindings.emplace(InputAction::Pause,       KeyBinding(KEY_ESCAPE, GAMEPAD_BUTTON_MIDDLE_RIGHT));
    m_Bindings.emplace(InputAction::DebugToggle, KeyBinding(KEY_F3, GAMEPAD_BUTTON_MIDDLE_LEFT));
}

void InputManager::Update() {
    // Detect active gamepad
    if (IsGamepadAvailable(0)) {
        m_GamepadId = 0;
    }
}

bool InputManager::IsActionPressed(InputAction action) const {
    auto it = m_Bindings.find(action);
    if (it == m_Bindings.end()) return false;

    bool keyState = IsKeyPressed(it->second.keyboardKey);
    bool padState = IsGamepadAvailable(m_GamepadId) && IsGamepadButtonPressed(m_GamepadId, it->second.gamepadButton);

    return keyState || padState;
}

bool InputManager::IsActionDown(InputAction action) const {
    auto it = m_Bindings.find(action);
    if (it == m_Bindings.end()) return false;

    bool keyState = IsKeyDown(it->second.keyboardKey);
    bool padState = IsGamepadAvailable(m_GamepadId) && IsGamepadButtonDown(m_GamepadId, it->second.gamepadButton);

    return keyState || padState;
}

bool InputManager::IsActionReleased(InputAction action) const {
    auto it = m_Bindings.find(action);
    if (it == m_Bindings.end()) return false;

    bool keyState = IsKeyReleased(it->second.keyboardKey);
    bool padState = IsGamepadAvailable(m_GamepadId) && IsGamepadButtonReleased(m_GamepadId, it->second.gamepadButton);

    return keyState || padState;
}

Vector2 InputManager::GetMoveVector() const {
    Vector2 dir{ 0.0f, 0.0f };

    if (IsActionDown(InputAction::MoveUp))    dir.y -= 1.0f;
    if (IsActionDown(InputAction::MoveDown))  dir.y += 1.0f;
    if (IsActionDown(InputAction::MoveLeft))  dir.x -= 1.0f;
    if (IsActionDown(InputAction::MoveRight)) dir.x += 1.0f;

    // Check gamepad analog stick if present
    if (IsGamepadAvailable(m_GamepadId)) {
        float axisX = GetGamepadAxisMovement(m_GamepadId, GAMEPAD_AXIS_LEFT_X);
        float axisY = GetGamepadAxisMovement(m_GamepadId, GAMEPAD_AXIS_LEFT_Y);

        constexpr float deadzone = 0.2f;
        if (std::abs(axisX) > deadzone) dir.x += axisX;
        if (std::abs(axisY) > deadzone) dir.y += axisY;
    }

    return Vector2LengthSqr(dir) > 0.0f ? Vector2Normalize(dir) : dir;
}

Vector2 InputManager::GetMouseVirtualPosition(Rectangle viewportRect, Vector2 virtualRes) const {
    Vector2 mousePos = GetMousePosition();

    // Map screen mouse position inside letterboxed viewport to offscreen resolution
    Vector2 virtualMouse{
        (mousePos.x - viewportRect.x) * (virtualRes.x / viewportRect.width),
        (mousePos.y - viewportRect.y) * (virtualRes.y / viewportRect.height)
    };

    return Vector2Clamp(virtualMouse, Vector2{0.0f, 0.0f}, virtualRes);
}

} // namespace Roguelike
