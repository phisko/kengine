#pragma once

#include <functional>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include "Point.hpp"

namespace putils { class BaseModule; }

namespace kengine {
    namespace packets {
        namespace KeyStatus {
            struct Query {
                sf::Keyboard::Key key;
                putils::BaseModule * sender;
            };
            struct Response { bool pressed; };
        }

        namespace MousePosition {
            struct Query { putils::BaseModule * sender; };
            struct Response { putils::Point2i pos; };
        }

        namespace MouseButtonStatus {
            struct Query {
                sf::Mouse::Button button;
                putils::BaseModule * sender;
            };
            struct Response { bool pressed; };
        }
    }
}