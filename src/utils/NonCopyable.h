#pragma once

namespace Roguelike {

class NonCopyable {
protected:
    constexpr NonCopyable() noexcept = default;
    ~NonCopyable() noexcept = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    NonCopyable(NonCopyable&&) noexcept = default;
    NonCopyable& operator=(NonCopyable&&) noexcept = default;
};

} // namespace Roguelike
