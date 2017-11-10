#pragma once

#include "Point.hpp"

namespace kengine { class GameObject; }
namespace putils { class BaseModule; }

namespace kengine
{
    namespace packets
    {
        namespace Position
        {
            struct Query
            {
                putils::Rect3d box;
                putils::BaseModule *sender;
            };

            struct Response
            {
                std::vector<kengine::GameObject *> objects;
            };
        }
    }
}