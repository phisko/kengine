#pragma once

#include <functional>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include "Point.hpp"

namespace putils { class BaseModule; }

namespace kengine
{
    namespace packets
    {
        struct RegisterKeyHandler
        {
            sf::Keyboard::Key key;
            std::function<void(sf::Keyboard::Key)> onPress;
            std::function<void(sf::Keyboard::Key)> onRelease;
        };

        struct RegisterMouseMovedHandler
        {
            std::function<void(const putils::Point2i &pos)> handler;
        };

        struct RegisterMouseButtonHandler
        {
            sf::Mouse::Button button;
            std::function<void(sf::Mouse::Button, int x, int y)> onPress;
            std::function<void(sf::Mouse::Button, int x, int y)> onRelease;
        };

        namespace KeyStatus
        {
            struct Query
            {
                sf::Keyboard::Key key;
                putils::BaseModule *sender;
            };
            struct Response { bool pressed; };
        }

        namespace MousePosition
        {
            struct Query { putils::BaseModule *sender; };
            struct Response { putils::Point2i pos; };
        }

        namespace MouseButtonStatus
        {
            struct Query
            {
                sf::Mouse::Button button;
                putils::BaseModule *sender;
            };
            struct Response { bool pressed; };
        }
    }
}