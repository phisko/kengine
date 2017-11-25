#pragma once

namespace kengine
{
    class GameObject;

    namespace packets
    {
        struct Collision
        {
            kengine::GameObject &first;
            kengine::GameObject &second;
        };
    }
}
