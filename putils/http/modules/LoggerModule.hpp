#pragma once

#include <fstream>
#include "mediator/Module.hpp"

#include "Packets.hpp"

class LoggerModule final : public putils::Module<LoggerModule, kia::packets::Log>
{
public:
    void handle(const kia::packets::Log &packet) noexcept
    {
        _ofs << packet.msg << std::endl;
        std::cout << packet.msg << std::endl;
    }

private:
    std::ofstream       _ofs { "log.txt" };
};
