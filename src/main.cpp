#include "core/Game.h"
#include "core/Logger.h"
#include <exception>

int main() {
    try {
        Roguelike::Game game;
        game.Run();
    } catch (const std::exception& e) {
        Roguelike::Logger::Error("Unhandled exception: {}", e.what());
        return -1;
    } catch (...) {
        Roguelike::Logger::Error("Unhandled unknown exception encountered!");
        return -1;
    }

    return 0;
}
