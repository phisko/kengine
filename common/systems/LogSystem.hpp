#pragma once

#include "System.hpp"
#include "common/packets/Log.hpp"
#include "EntityManager.hpp"

namespace kengine
{
    class LogSystem : public kengine::System<LogSystem, packets::Log>
    {
        // Ctor
    public:
        LogSystem(kengine::EntityManager &) {}

        // Packet handlers
    public:
        void handle(const packets::Log &packet) const noexcept
        {
            std::cout << packet.msg << std::endl;
        }
    };
}
