#include "physics/SpatialHashGrid.h"
#include <cmath>
#include <algorithm>

namespace Roguelike {

SpatialHashGrid::SpatialHashGrid(float cellSize)
    : m_CellSize(cellSize > 0.0f ? cellSize : 32.0f)
{
}

void SpatialHashGrid::Clear() {
    m_Grid.clear();
}

int64_t SpatialHashGrid::HashCell(int cellX, int cellY) const noexcept {
    // 64-bit spatial hash combining X and Y grid coordinates
    return (static_cast<int64_t>(cellX) << 32) ^ static_cast<int64_t>(cellY);
}

void SpatialHashGrid::GetCellCoords(Vector2 position, int& outX, int& outY) const noexcept {
    outX = static_cast<int>(std::floor(position.x / m_CellSize));
    outY = static_cast<int>(std::floor(position.y / m_CellSize));
}

void SpatialHashGrid::Insert(const SpatialEntity& entity) {
    int minX, minY, maxX, maxY;
    GetCellCoords(Vector2{ entity.position.x - entity.radius, entity.position.y - entity.radius }, minX, minY);
    GetCellCoords(Vector2{ entity.position.x + entity.radius, entity.position.y + entity.radius }, maxX, maxY);

    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            m_Grid[HashCell(x, y)].push_back(entity);
        }
    }
}

std::vector<SpatialEntity> SpatialHashGrid::Query(Vector2 position, float queryRadius) const {
    std::vector<SpatialEntity> result;
    int minX, minY, maxX, maxY;
    GetCellCoords(Vector2{ position.x - queryRadius, position.y - queryRadius }, minX, minY);
    GetCellCoords(Vector2{ position.x + queryRadius, position.y + queryRadius }, maxX, maxY);

    std::unordered_map<uint32_t, bool> visited;

    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            auto it = m_Grid.find(HashCell(x, y));
            if (it != m_Grid.end()) {
                for (const auto& entity : it->second) {
                    if (!visited[entity.id]) {
                        visited[entity.id] = true;
                        result.push_back(entity);
                    }
                }
            }
        }
    }

    return result;
}

} // namespace Roguelike
