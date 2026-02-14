#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdlib>

struct Entity {
    sf::Vector2f position;
    float radius;
    sf::Color color;
    int id; // For debugging/tracking

    Entity(float x, float y, float r, int entityId = 0)
        : position(x, y), radius(r), id(entityId) {
        // Random color for visualization
        color = sf::Color(
            rand() % 200 + 55,
            rand() % 200 + 55,
            rand() % 200 + 55
        );
    }

    // Check if this entity collides with another
    bool collidesWith(const Entity& other) const {
        float dx = position.x - other.position.x;
        float dy = position.y - other.position.y;
        float distanceSquared = dx * dx + dy * dy;
        float radiusSum = radius + other.radius;
        return distanceSquared < (radiusSum * radiusSum);
    }
};

// Moving entity with velocity
struct MovingEntity : public Entity {
    sf::Vector2f velocity;

    MovingEntity(float x, float y, float r, int id)
        : Entity(x, y, r, id) {
        velocity.x = (rand() % 200 - 100) / 50.0f;
        velocity.y = (rand() % 200 - 100) / 50.0f;
    }

    void update(float width, float height, float speedMultiplier = 1.0f) {  // Add parameter
        // Apply speed multiplier
        sf::Vector2f scaledVelocity = velocity * speedMultiplier;

        // Update position
        position += scaledVelocity;

        // Bounce off LEFT wall
        if (position.x - radius < 0) {
            position.x = radius;
            velocity.x = std::abs(velocity.x);
        }
        // Bounce off RIGHT wall
        if (position.x + radius > width) {
            position.x = width - radius;
            velocity.x = -std::abs(velocity.x);
        }

        // Bounce off TOP wall
        if (position.y - radius < 0) {
            position.y = radius;
            velocity.y = std::abs(velocity.y);
        }
        // Bounce off BOTTOM wall
        if (position.y + radius > height) {
            position.y = height - radius;
            velocity.y = -std::abs(velocity.y);
        }
    }
};
