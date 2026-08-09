#pragma once
#include <string_view>
#include <format>
#include <iostream>

namespace Roguelike {

enum class LogLevel {
    Info,
    Warning,
    Error,
    Debug
};

class Logger {
public:
    template<typename... Args>
    static void Info(std::format_string<Args...> fmt, Args&&... args) {
        Log(LogLevel::Info, std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void Warn(std::format_string<Args...> fmt, Args&&... args) {
        Log(LogLevel::Warning, std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void Error(std::format_string<Args...> fmt, Args&&... args) {
        Log(LogLevel::Error, std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void Debug(std::format_string<Args...> fmt, Args&&... args) {
#ifdef _DEBUG
        Log(LogLevel::Debug, std::format(fmt, std::forward<Args>(args)...));
#else
        (void)fmt;
#endif
    }

private:
    static void Log(LogLevel level, std::string_view message);
};

} // namespace Roguelike
