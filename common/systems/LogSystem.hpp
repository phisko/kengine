#pragma once

#include "System.hpp"
#include "common/packets/Log.hpp"
#include "EntityManager.hpp"

class LogSystem : public kengine::System<LogSystem, kengine::IgnoreComponents,
        packets::Log>
{
    // Ctor
public:
    LogSystem(kengine::EntityManager &) = default;

    // Packet handlers
public:
    void handle(const packets::Log &packet) const noexcept
    {
        std::cout << packet.msg << std::endl;
    }
};
