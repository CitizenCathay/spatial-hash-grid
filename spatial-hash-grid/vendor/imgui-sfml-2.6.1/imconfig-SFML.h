#pragma once

#define IMGUI_USER_CONFIG "imconfig-SFML.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

#define IM_VEC2_CLASS_EXTRA                                             \
        ImVec2(const sf::Vector2f& v) { x = v.x; y = v.y; }             \
        operator sf::Vector2f() const { return sf::Vector2f(x, y); }

#define IM_VEC4_CLASS_EXTRA                                             \
        ImVec4(const sf::Color& c)                                      \
            : x(c.r / 255.f), y(c.g / 255.f), z(c.b / 255.f), w(c.a / 255.f) {} \
        operator sf::Color() const {                                    \
            return sf::Color(                                           \
                static_cast<sf::Uint8>(x * 255.f),                      \
                static_cast<sf::Uint8>(y * 255.f),                      \
                static_cast<sf::Uint8>(z * 255.f),                      \
                static_cast<sf::Uint8>(w * 255.f));                     \
        }