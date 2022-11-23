#pragma once

// stl
#include <memory>

// sfml
#include <SFML/Graphics/RenderWindow.hpp>

namespace kengine {
    struct SFMLWindowComponent {
        std::unique_ptr<sf::RenderWindow> window;
    };
}