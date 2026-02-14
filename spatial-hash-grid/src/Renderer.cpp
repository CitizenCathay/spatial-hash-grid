/*!*****************************************************************************
\file       Renderer.cpp
\author     Choi Meng Yew (2401822@sit.singaporetech.edu.sg)
\co-author  Alyssa Cerrero Nicole Alejandro
\co-author  Chan Qi Ying
\par        Course: CSD2183
\par        Homework 1: Spatial Hash Grid
\date       14th February 2026
\brief      Rendering utilities for spatial hash grid visualization using SFML.
            Includes batched rendering for improved performance with many entities.
*******************************************************************************/

#include "Renderer.h"
#include <set>
#include <cmath>

/*!*****************************************************************************
\brief      Draw the spatial hash grid lines

\param[in]  window
            SFML render window to draw to
\param[in]  cellSize
            Size of each grid cell in pixels
\param[in]  width
            Width of the drawing area
\param[in]  height
            Height of the drawing area

\details    Draws semi-transparent gray grid lines to visualize the spatial
            hash cell boundaries.
*******************************************************************************/
void Renderer::drawGrid(sf::RenderWindow& window, float cellSize, float width, float height) {
    // Vertical lines
    for (float x = 0; x < width; x += cellSize) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(x, 0), sf::Color(60, 60, 60, 128)),
            sf::Vertex(sf::Vector2f(x, height), sf::Color(60, 60, 60, 128))
        };
        window.draw(line, 2, sf::Lines);
    }

    // Horizontal lines
    for (float y = 0; y < height; y += cellSize) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(0, y), sf::Color(60, 60, 60, 128)),
            sf::Vertex(sf::Vector2f(width, y), sf::Color(60, 60, 60, 128))
        };
        window.draw(line, 2, sf::Lines);
    }
}

/*!*****************************************************************************
\brief      Highlight grid cells that contain entities

\param[in]  window
            SFML render window to draw to
\param[in]  grid
            Spatial hash grid (unused in current implementation)
\param[in]  entities
            Vector of entities to check cell occupancy
\param[in]  cellSize
            Size of each grid cell in pixels

\details    Draws semi-transparent blue rectangles over cells containing one or
            more entities. Uses a set to avoid highlighting the same cell multiple
            times when multiple entities occupy it.
*******************************************************************************/
void Renderer::drawActiveCells(sf::RenderWindow& window, const SpatialHashGrid& grid,
    const std::vector<MovingEntity>& entities, float cellSize) {
    std::set<std::pair<int, int>> activeCells;

    // Find which cells have entities
    for (const auto& entity : entities) {
        int cellX = static_cast<int>(std::floor(entity.position.x / cellSize));
        int cellY = static_cast<int>(std::floor(entity.position.y / cellSize));
        activeCells.insert({ cellX, cellY });
    }

    // Draw highlighted rectangles for active cells
    for (const auto& [cellX, cellY] : activeCells) {
        sf::RectangleShape rect(sf::Vector2f(cellSize, cellSize));
        rect.setPosition(cellX * cellSize, cellY * cellSize);
        rect.setFillColor(sf::Color(80, 120, 200, 30)); // Light blue, transparent
        rect.setOutlineThickness(1.0f);
        rect.setOutlineColor(sf::Color(100, 150, 255, 100));
        window.draw(rect);
    }
}

/*!*****************************************************************************
\brief      Draw entities as circles (non-batched)

\param[in]  window
            SFML render window to draw to
\param[in]  entities
            Vector of entities to render

\details    Draws each entity individually with white outline. Performance
            degrades with many entities due to multiple draw calls. Use
            drawEntitiesBatched() for better performance.

\deprecated Use drawEntitiesBatched() for improved performance
*******************************************************************************/
void Renderer::drawEntities(sf::RenderWindow& window, const std::vector<MovingEntity>& entities) {
    for (const auto& entity : entities) {
        sf::CircleShape circle(entity.radius);
        circle.setPosition(entity.position.x - entity.radius,
            entity.position.y - entity.radius);
        circle.setFillColor(entity.color);
        circle.setOutlineThickness(1.0f);
        circle.setOutlineColor(sf::Color::White);
        window.draw(circle);
    }
}

/*!*****************************************************************************
\brief      Draw red lines between colliding entities

\param[in]  window
            SFML render window to draw to
\param[in]  collisionPairs
            Vector of entity pointer pairs that are colliding

\details    Visualizes collision detection results by drawing red lines
            connecting the centers of colliding entities.
*******************************************************************************/
void Renderer::drawCollisions(sf::RenderWindow& window,
    const std::vector<std::pair<Entity*, Entity*>>& collisionPairs) {
    for (const auto& [a, b] : collisionPairs) {
        sf::Vertex line[] = {
            sf::Vertex(a->position, sf::Color::Red),
            sf::Vertex(b->position, sf::Color::Red)
        };
        window.draw(line, 2, sf::Lines);
    }
}

/*!*****************************************************************************
\brief      Draw all entities using batched rendering (optimized)

\param[in]  window
            SFML render window to draw to
\param[in]  entities
            Vector of entities to render

\details    Uses vertex arrays to render all entities in a single GPU draw call.
            Each circle is approximated using 20 triangles arranged in a fan
            pattern. This approach significantly improves performance compared
            to individual CircleShape draw calls.

\par        Performance
            - Individual drawing: n draw calls for n entities
            - Batched drawing: 1 draw call for n entities
            - Allows 60 FPS with 5000+ entities vs ~15 FPS with individual drawing
*******************************************************************************/
void Renderer::drawEntitiesBatched(sf::RenderWindow& window, const std::vector<MovingEntity>& entities) {
    const int CIRCLE_SEGMENTS = 20; // Number of triangles per circle

    // Pre-allocate vertex array for all circles
    // Each circle = CIRCLE_SEGMENTS triangles, each triangle = 3 vertices
    sf::VertexArray vertices(sf::Triangles, entities.size() * CIRCLE_SEGMENTS * 3);

    size_t vertexIndex = 0;

    for (const auto& entity : entities) {
        const float radius = entity.radius;
        const sf::Vector2f center = entity.position;
        const sf::Color color = entity.color;

        // Create circle using triangle fan
        for (int i = 0; i < CIRCLE_SEGMENTS; ++i) {
            float angle1 = (i / static_cast<float>(CIRCLE_SEGMENTS)) * 2.0f * 3.14159f;
            float angle2 = ((i + 1) / static_cast<float>(CIRCLE_SEGMENTS)) * 2.0f * 3.14159f;

            // Triangle: center + two edge points
            vertices[vertexIndex++] = sf::Vertex(center, color);
            vertices[vertexIndex++] = sf::Vertex(
                sf::Vector2f(center.x + radius * std::cos(angle1),
                    center.y + radius * std::sin(angle1)),
                color
            );
            vertices[vertexIndex++] = sf::Vertex(
                sf::Vector2f(center.x + radius * std::cos(angle2),
                    center.y + radius * std::sin(angle2)),
                color
            );
        }
    }

    // Draw ALL circles in ONE call
    window.draw(vertices);
}
