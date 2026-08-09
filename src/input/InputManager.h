#pragma once
#include "utils/NonCopyable.h"
#include "input/InputActions.h"
#include <raylib.h>
#include <unordered_map>

namespace Roguelike {

class InputManager : public NonCopyable {
public:
    InputManager();

    void Update();

    [[nodiscard]] bool IsActionPressed(InputAction action) const;
    [[nodiscard]] bool IsActionDown(InputAction action) const;
    [[nodiscard]] bool IsActionReleased(InputAction action) const;

    [[nodiscard]] Vector2 GetMoveVector() const;
    [[nodiscard]] Vector2 GetMouseVirtualPosition(Rectangle viewportRect, Vector2 virtualRes) const;

private:
    struct KeyBinding {
        KeyboardKey keyboardKey;
        GamepadButton gamepadButton;
        constexpr KeyBinding() = default;
        constexpr KeyBinding(KeyboardKey k, GamepadButton g) : keyboardKey(k), gamepadButton(g) {}
    };

    std::unordered_map<InputAction, KeyBinding> m_Bindings;
    int m_GamepadId{0};
};

} // namespace Roguelike
