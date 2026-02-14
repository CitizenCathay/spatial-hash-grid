/*!*****************************************************************************
\file       main.cpp
\author     Choi Meng Yew (2401822@sit.singaporetech.edu.sg)
\co-author  Alyssa Cerrero Nicole Alejandro
\co-author  Chan Qi Ying
\par        Course: CSD2183
\par        Homework 1: Spatial Hash Grid
\date       14th February 2026

\brief      Entry point for Homework 1: Spatial Hash Grid demonstration.

            This application visualizes real-time collision detection
            between moving circular entities using two approaches:

                1) Brute-force pairwise collision checking (O(n^2))
                2) Spatial Hash Grid partitioning (O(n) average case)

            The program allows runtime comparison between both methods,
            displaying collision checks, active grid cells, and FPS to
            illustrate performance differences as entity count scales.

            Features:
                - Dynamic entity spawning and resizing of viewport
                - Adjustable grid cell size
                - Real-time performance statistics
                - Visualization of grid cells, active cells, and collisions
                - Toggle between brute-force and spatial hashing modes

            Purpose:
                To demonstrate how spatial partitioning reduces the
                computational complexity of broad-phase collision detection
                in large-scale simulations.

*******************************************************************************/

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <vector>
#include <string>
#include "SpatialHashGrid.h"
#include "Entity.h"
#include "Renderer.h"

/*!*****************************************************************************
\brief      Create and initialize a set of moving entities

\param[out] entities
            Vector to populate with new entities (cleared before adding)
\param[in]  count
            Number of entities to create
\param[in]  width
            Width of spawn area
\param[in]  height
            Height of spawn area

\details    Generates entities with random positions within the specified
            bounds and random radii between 8-23 pixels. Each entity is
            assigned a unique ID based on its index.
*******************************************************************************/
void createEntities(std::vector<MovingEntity>& entities, int count, int width, int height) {
    entities.clear();
    for (int i = 0; i < count; ++i) {
        float x = static_cast<float>(rand() % width);
        float y = static_cast<float>(rand() % height);
        float radius = 8.0f + static_cast<float>(rand() % 15);
        entities.emplace_back(x, y, radius, i);
    }
}

/*!*****************************************************************************
\brief      Perform brute-force collision detection

\param[in]  entities
            Vector of entities to check for collisions
\param[out] checkCount
            Reference to store the number of collision checks performed

\return     Vector of entity pointer pairs that are colliding

\details    Checks every entity against every other entity using nested loops.
            This demonstrates the O(n²) complexity of naive collision detection.

\par        Complexity
            Time: O(n²) where n is the number of entities
            Space: O(k) where k is the number of collision pairs
*******************************************************************************/
std::vector<std::pair<Entity*, Entity*>> bruteForceCollisions(std::vector<MovingEntity>& entities, int& checkCount) {
    std::vector<std::pair<Entity*, Entity*>> pairs;
    checkCount = 0;
    for (size_t i = 0; i < entities.size(); ++i) {
        for (size_t j = i + 1; j < entities.size(); ++j) {
            checkCount++;
            if (entities[i].collidesWith(entities[j])) {
                pairs.push_back({ &entities[i], &entities[j] });
            }
        }
    }
    return pairs;
}

/*!*****************************************************************************
\brief      Application entry point

\return     0 on successful exit

\details    Initializes SFML window with ImGui integration, creates the initial
            set of entities, and enters the main event loop. The loop handles:
            - User input (mouse clicks, window events, ImGui interactions)
            - Entity updates (movement, wall bouncing)
            - Collision detection (spatial hash or brute force based on toggle)
            - Rendering (grid, entities, collision visualization, UI panel)
            - Performance metrics (FPS calculation and display)

\par        Window Layout
            - Left side: 1000x720 viewport for simulation
            - Right side: 320-pixel wide ImGui control panel
            - Viewport and panel resize dynamically with window

\par        Controls
            - Left click in viewport: Spawn new entity at cursor
            - ESC or close button: Exit application
            - ImGui sliders/buttons: Adjust parameters in real-time
*******************************************************************************/
int main() {
    // Window dimensions with panel on the right (now dynamic)
    const int PANEL_WIDTH = 320;
    int viewportWidth = 1000;  // Not const - can change on resize
    int windowWidth = viewportWidth + PANEL_WIDTH;
    int windowHeight = 720;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight),
        "Spatial Hash Grid Demo", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    sf::Clock fpsClock;
    int fpsFrameCount = 0;
    float fps = 0.0f;

    // UI Controls
    float cellSize = 100.0f;
    int entityCount = 100;
    float entitySpeed = 1.0f;
    bool showGrid = true;
    bool showActiveCells = true;
    bool showCollisions = true;
    bool useSpatialHash = true;

    int bruteForceChecks = 0;
    int spatialHashChecks = 0;

    SpatialHashGrid spatialHash(cellSize);
    std::vector<MovingEntity> entities;
    createEntities(entities, entityCount, viewportWidth, windowHeight);

    std::cout << "Created " << entities.size() << " moving entities" << std::endl;
    std::cout << "Cell size: " << cellSize << std::endl;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();

            // Handle window resize
            if (event.type == sf::Event::Resized) {
                windowWidth = event.size.width;
                windowHeight = event.size.height;
                viewportWidth = windowWidth - PANEL_WIDTH;

                // Update SFML view
                window.setView(sf::View(sf::FloatRect(0, 0, windowWidth, windowHeight)));

                std::cout << "Resized - Viewport: " << viewportWidth << "x" << windowHeight << std::endl;
            }

            // Mouse click to spawn entity (only in viewport area)
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (!ImGui::GetIO().WantCaptureMouse) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (mousePos.x < viewportWidth) {  // Only spawn in viewport
                        float radius = 10.0f + static_cast<float>(rand() % 15);
                        entities.emplace_back(static_cast<float>(mousePos.x),
                            static_cast<float>(mousePos.y),
                            radius,
                            entities.size());
                            entityCount = static_cast<int>(entities.size());
                    }
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // === DOCKED CONTROL PANEL ===
        ImGui::SetNextWindowPos(ImVec2(static_cast<float>(viewportWidth), 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(PANEL_WIDTH), static_cast<float>(windowHeight)), ImGuiCond_Always);

        ImGui::Begin("Spatial Hash Controls", nullptr,
            ImGuiWindowFlags_NoResize |        // Removed NoMove!
            ImGuiWindowFlags_NoCollapse);

        // Collision Detection Mode
        ImGui::Text("Collision Detection Mode");
        ImGui::Separator();
        if (ImGui::RadioButton("Spatial Hash (O(n))", useSpatialHash)) {
            useSpatialHash = true;
        }
        if (ImGui::RadioButton("Brute Force (O(n²))", !useSpatialHash)) {
            useSpatialHash = false;
        }

        ImGui::Spacing();

        // Entity Settings
        ImGui::Text("Entity Settings");
        ImGui::Separator();

        if (ImGui::SliderInt("Entity Count", &entityCount, 10, 2000)) {
            createEntities(entities, entityCount, viewportWidth, windowHeight);
        }

        ImGui::SliderFloat("Speed Multiplier", &entitySpeed, 0.1f, 5.0f, "%.1fx");

        if (ImGui::Button("Add 100 Entities", ImVec2(-1, 0))) {
            for (int i = 0; i < 100; ++i) {
                float x = static_cast<float>(rand() % viewportWidth);
                float y = static_cast<float>(rand() % windowHeight);
                float radius = 8.0f + static_cast<float>(rand() % 15);
                entities.emplace_back(x, y, radius, entities.size());
            }
            entityCount = static_cast<int>(entities.size());
        }

        if (ImGui::Button("Add 500 Entities", ImVec2(-1, 0))) {
            for (int i = 0; i < 500; ++i) {
                float x = static_cast<float>(rand() % viewportWidth);
                float y = static_cast<float>(rand() % windowHeight);
                float radius = 8.0f + static_cast<float>(rand() % 15);
                entities.emplace_back(x, y, radius, entities.size());
            }
            entityCount = static_cast<int>(entities.size());
        }

        if (ImGui::Button("Reset to 100", ImVec2(-1, 0))) {
            createEntities(entities, 100, viewportWidth, windowHeight);
            entityCount = 100;
        }

        ImGui::TextWrapped("Click on viewport to spawn entity");

        ImGui::Spacing();

        // Grid Settings
        ImGui::Text("Grid Settings");
        ImGui::Separator();

        if (ImGui::SliderFloat("Cell Size", &cellSize, 50.0f, 1200.0f, "%.0f px")) {
            spatialHash = SpatialHashGrid(cellSize);
        }

        ImGui::Checkbox("Show Grid", &showGrid);
        ImGui::Checkbox("Show Active Cells", &showActiveCells);
        ImGui::Checkbox("Show Collisions", &showCollisions);

        ImGui::Spacing();

        // Performance Stats
        ImGui::Text("Performance Statistics");
        ImGui::Separator();
        ImGui::Text("FPS: %d", static_cast<int>(fps));
        ImGui::Text("Entities: %zu", entities.size());
        ImGui::Text("Active Cells: %zu", spatialHash.getActiveCellCount());

        if (useSpatialHash) {
            ImGui::Text("Collision Checks: %d", spatialHashChecks);
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Using Spatial Hash");
        }
        else {
            ImGui::Text("Collision Checks: %d", bruteForceChecks);
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Using Brute Force");
        }

        ImGui::Spacing();
        ImGui::Text("Theoretical Complexity:");
        ImGui::BulletText("Spatial Hash: O(n) average");
        ImGui::BulletText("Brute Force: O(n^2) = %d checks",
            (int)(entities.size() * (entities.size() - 1) / 2));

        ImGui::End();

        // === UPDATE ENTITIES ===
        for (auto& entity : entities) {
            entity.update(viewportWidth, windowHeight, entitySpeed);
        }

        // === COLLISION DETECTION ===
        std::vector<std::pair<Entity*, Entity*>> collisionPairs;

        if (useSpatialHash) {
            spatialHash.clear();
            for (auto& entity : entities) {
                spatialHash.insert(&entity);
            }
            collisionPairs = spatialHash.getCollisionPairs();
            spatialHashChecks = spatialHash.getCollisionCheckCount();
        }
        else {
            collisionPairs = bruteForceCollisions(entities, bruteForceChecks);
        }

        // === CALCULATE FPS ===
        fpsFrameCount++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
            fps = fpsFrameCount / fpsClock.restart().asSeconds();
            fpsFrameCount = 0;
        }

        // === RENDER ===
        window.clear(sf::Color(30, 30, 30));

        // Draw viewport separator line
        sf::Vertex separator[] = {
            sf::Vertex(sf::Vector2f(static_cast<float>(viewportWidth), 0), sf::Color(80, 80, 80)),
            sf::Vertex(sf::Vector2f(static_cast<float>(viewportWidth), static_cast<float>(windowHeight)), sf::Color(80, 80, 80))
        };
        window.draw(separator, 2, sf::Lines);

        if (showGrid)
            Renderer::drawGrid(window, cellSize, static_cast<float>(viewportWidth), static_cast<float>(windowHeight));

        if (showActiveCells)
            Renderer::drawActiveCells(window, spatialHash, entities, cellSize);

        Renderer::drawEntitiesBatched(window, entities);

        if (showCollisions)
            Renderer::drawCollisions(window, collisionPairs);

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
