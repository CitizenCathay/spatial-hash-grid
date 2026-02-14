#pragma once
#include <unordered_map>
#include <vector>
#include "Entity.h"

class SpatialHashGrid {
private:
    float cellSize;
    // Hash table: key = cell hash, value = list of entities in that cell
    std::unordered_map<int, std::vector<Entity*>> grid;

    // Hash function: converts 2D position to cell hash
    int hash(float x, float y) const;

    // Get cell coordinates from world position
    int getCellX(float x) const;
    int getCellY(float y) const;

public:
    SpatialHashGrid(float cellSize);

    // Insert entity into grid
    void insert(Entity* entity);

    // Remove all entities and clear grid
    void clear();

    // Query entities near a position
    std::vector<Entity*> query(float x, float y, float radius) const;

    // Get all potential collision pairs using spatial hash
    std::vector<std::pair<Entity*, Entity*>> getCollisionPairs() const;

    // Debug: get number of cells in use
    size_t getActiveCellCount() const { return grid.size(); }

    // Debug: get total entities across all cells
    size_t getTotalEntityReferences() const;

    int getCollisionCheckCount() const;
};