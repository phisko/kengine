#pragma once

#include "Point.hpp"

namespace kengine { class GameObject; }
namespace putils { class BaseModule; }

namespace kengine {
    namespace packets {
        namespace Position {
            struct Query {
                putils::Rect3f box;
                putils::BaseModule * sender;
            };

            struct Response {
                std::vector<kengine::GameObject *> objects;
            };
        }
    }
}