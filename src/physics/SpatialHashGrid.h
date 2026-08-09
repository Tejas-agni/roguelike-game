#pragma once
#include <raylib.h>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace Roguelike {

struct SpatialEntity {
    uint32_t id;
    Vector2 position;
    float radius;
    uint32_t type; // 0 = Player, 1 = Enemy, 2 = Projectile, 3 = Gem
};

class SpatialHashGrid {
public:
    explicit SpatialHashGrid(float cellSize = 32.0f);

    void Clear();
    void Insert(const SpatialEntity& entity);

    // Get nearby entity IDs within radius of a position
    [[nodiscard]] std::vector<SpatialEntity> Query(Vector2 position, float queryRadius) const;

private:
    [[nodiscard]] int64_t HashCell(int cellX, int cellY) const noexcept;
    void GetCellCoords(Vector2 position, int& outX, int& outY) const noexcept;

    float m_CellSize;
    std::unordered_map<int64_t, std::vector<SpatialEntity>> m_Grid;
};

} // namespace Roguelike
