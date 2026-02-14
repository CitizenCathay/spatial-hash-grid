#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "SpatialHashGrid.h"
#include "Entity.h"

class Renderer {
public:
    static void drawGrid(sf::RenderWindow& window, float cellSize, float width, float height);
    static void drawActiveCells(sf::RenderWindow& window, const SpatialHashGrid& grid,
        const std::vector<MovingEntity>& entities, float cellSize);

    // Old method (slow - one draw call per circle)
    static void drawEntities(sf::RenderWindow& window, const std::vector<MovingEntity>& entities);

    // NEW: Batch rendering method (fast - one draw call total)
    static void drawEntitiesBatched(sf::RenderWindow& window, const std::vector<MovingEntity>& entities);

    static void drawCollisions(sf::RenderWindow& window,
        const std::vector<std::pair<Entity*, Entity*>>& collisionPairs);
};