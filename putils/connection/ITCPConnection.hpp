#pragma once

#include <string>

namespace putils
{
    class ITCPConnection
    {
    public:
        virtual ~ITCPConnection() = default;

    public:
        virtual void send(const std::string &msg) = 0;
        virtual void send(const char *data, size_t length) = 0;
        virtual std::string receive() = 0;
        virtual void receive(char *dest, size_t length) = 0;
    };
}