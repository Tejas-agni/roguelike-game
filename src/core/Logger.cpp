#include "core/Logger.h"
#include <raylib.h>
#include <chrono>
#include <iomanip>

namespace Roguelike {

void Logger::Log(LogLevel level, std::string_view message) {
    const char* prefix = "[INFO]";
    int raylibLogLevel = LOG_INFO;

    switch (level) {
        case LogLevel::Info:
            prefix = "[INFO]";
            raylibLogLevel = LOG_INFO;
            break;
        case LogLevel::Warning:
            prefix = "[WARN]";
            raylibLogLevel = LOG_WARNING;
            break;
        case LogLevel::Error:
            prefix = "[ERROR]";
            raylibLogLevel = LOG_ERROR;
            break;
        case LogLevel::Debug:
            prefix = "[DEBUG]";
            raylibLogLevel = LOG_DEBUG;
            break;
    }

    TraceLog(raylibLogLevel, "%s %.*s", prefix, static_cast<int>(message.size()), message.data());
}

} // namespace Roguelike
