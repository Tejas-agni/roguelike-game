#pragma once
#include <string>

namespace Roguelike {

template<typename T>
struct ResourceHandle {
    std::string id;
    bool isValid{false};
};

} // namespace Roguelike
