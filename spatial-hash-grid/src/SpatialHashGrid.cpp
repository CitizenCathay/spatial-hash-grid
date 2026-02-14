/*!*****************************************************************************
\file       SpatialHashGrid.cpp
\author     Choi Meng Yew (2401822@sit.singaporetech.edu.sg)
\co-author  Alyssa Cerrero Nicole Alejandro
\co-author  Chan Qi Ying
\par        Course: CSD2183
\par        Homework 1: Spatial Hash Grid
\date       14th February 2026
\brief      Implementation of spatial hash grid for efficient 2D collision
            detection using hash table with chaining collision resolution.
*******************************************************************************/

#include "SpatialHashGrid.h"
#include <cmath>
#include <set>

/*!*****************************************************************************
\brief      Constructor for SpatialHashGrid

\param[in]  cellSize
            Size of each grid cell in pixels
*******************************************************************************/
SpatialHashGrid::SpatialHashGrid(float cellSize)
    : cellSize(cellSize) {
}

/*!*****************************************************************************
\brief      Convert world X coordinate to grid cell X index

\param[in]  x
            World space X coordinate

\return     Grid cell X index
*******************************************************************************/
int SpatialHashGrid::getCellX(float x) const {
    return static_cast<int>(std::floor(x / cellSize));
}

/*!*****************************************************************************
\brief      Convert world Y coordinate to grid cell Y index

\param[in]  y
            World space Y coordinate

\return     Grid cell Y index
*******************************************************************************/
int SpatialHashGrid::getCellY(float y) const {
    return static_cast<int>(std::floor(y / cellSize));
}

/*!*****************************************************************************
\brief      Hash function to convert 2D cell coordinates to hash value

\param[in]  x
            World space X coordinate
\param[in]  y
            World space Y coordinate

\return     Hash value for the grid cell

\details    Uses prime number multiplication to distribute 2D coordinates
            uniformly across hash space. Prime numbers 73856093 and 19349663
            reduce hash collisions.
*******************************************************************************/
int SpatialHashGrid::hash(float x, float y) const {
    int cellX = getCellX(x);
    int cellY = getCellY(y);

    // Spatial hash function using prime numbers
    // This distributes 2D coordinates across hash space
    return cellX * 73856093 ^ cellY * 19349663;
}

/*!*****************************************************************************
\brief      Insert an entity into the spatial hash grid

\param[in]  entity
            Pointer to entity to insert

\details    Calculates the hash for the entity's position and inserts it into
            the appropriate bucket using chaining collision resolution.
*******************************************************************************/
void SpatialHashGrid::insert(Entity* entity) {
    // Calculate which cell this entity belongs to
    int cellHash = hash(entity->position.x, entity->position.y);

    // Insert into the appropriate bucket (chaining collision resolution)
    grid[cellHash].push_back(entity);
}

/*!*****************************************************************************
\brief      Clear all entities from the grid

\details    Removes all entities from all hash table buckets. Should be called
            each frame before rebuilding the grid with updated entity positions.
*******************************************************************************/
void SpatialHashGrid::clear() {
    grid.clear();
}

/*!*****************************************************************************
\brief      Query entities near a position

\param[in]  x
            Query position X coordinate
\param[in]  y
            Query position Y coordinate
\param[in]  radius
            Query radius (currently unused, searches full 3x3 cell grid)

\return     Vector of all entities in the cell and neighboring cells

\details    Searches a 3x3 grid of cells centered on the query position to
            find all nearby entities. This accounts for entities near cell
            boundaries.
*******************************************************************************/
std::vector<Entity*> SpatialHashGrid::query(float x, float y, float radius) const {
    std::vector<Entity*> result;

    // Check the cell and neighboring cells
    int cellX = getCellX(x);
    int cellY = getCellY(y);

    // Check 3x3 grid of cells around the query position
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            int neighborHash = (cellX + dx) * 73856093 ^ (cellY + dy) * 19349663;

            auto it = grid.find(neighborHash);
            if (it != grid.end()) {
                // Add all entities from this cell
                for (Entity* entity : it->second) {
                    result.push_back(entity);
                }
            }
        }
    }

    return result;
}

/*!*****************************************************************************
\brief      Get all collision pairs using spatial hash optimization

\return     Vector of entity pointer pairs that are colliding

\details    Uses spatial hash grid to efficiently detect collisions. Only checks
            entities within the same cell, avoiding O(n²) brute force approach.
            Maintains a set of checked pairs to prevent duplicate collision
            detections.

\par        Complexity
            - Average case: O(n) where n is number of entities
            - Worst case: O(n²) if all entities in one cell
*******************************************************************************/
std::vector<std::pair<Entity*, Entity*>> SpatialHashGrid::getCollisionPairs() const {
    std::vector<std::pair<Entity*, Entity*>> pairs;
    std::set<std::pair<int, int>> checkedPairs; // Avoid duplicate checks

    // For each cell
    for (const auto& [cellHash, entities] : grid) {
        // Check all pairs within this cell
        for (size_t i = 0; i < entities.size(); ++i) {
            for (size_t j = i + 1; j < entities.size(); ++j) {
                Entity* a = entities[i];
                Entity* b = entities[j];

                // Create ordered pair to avoid duplicates
                int id1 = std::min(a->id, b->id);
                int id2 = std::max(a->id, b->id);

                if (checkedPairs.insert({ id1, id2 }).second) {
                    if (a->collidesWith(*b)) {
                        pairs.push_back({ a, b });
                    }
                }
            }
        }
    }

    return pairs;
}

/*!*****************************************************************************
\brief      Get total number of entity references across all cells

\return     Total count of entity pointers stored in grid

\details    Useful for debugging and performance analysis. Note that if entities
            span multiple cells, they may be counted multiple times.
*******************************************************************************/
size_t SpatialHashGrid::getTotalEntityReferences() const {
    size_t total = 0;
    for (const auto& [cellHash, entities] : grid) {
        total += entities.size();
    }
    return total;
}

/*!*****************************************************************************
\brief      Calculate number of collision checks performed with current configuration

\return     Number of pairwise collision checks required

\details    Counts all entity pairs within each cell. This represents the actual
            work done by the spatial hash collision detection. Updates in real-time
            as entities move between cells.
*******************************************************************************/
int SpatialHashGrid::getCollisionCheckCount() const {
    int checkCount = 0;
    for (const auto& [cellHash, entities] : grid) {
        // For each cell, we check all pairs: n*(n-1)/2
        int n = static_cast<int>(entities.size());
        if (n > 1) {
            checkCount += (n * (n - 1)) / 2;
        }
    }
    return checkCount;
}
