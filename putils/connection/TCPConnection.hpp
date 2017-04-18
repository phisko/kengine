#pragma once

#include <unistd.h>
#include <iostream>
#include "ATCPConnection.hpp"

namespace putils
{
    class TCPConnection final : public ATCPConnection
    {
    public:
        TCPConnection(const std::string &host, short port, bool verbose = false)
                : ATCPConnection(host, port, verbose) {}

    private:
        int doRead(int fd, char *dest, int length) noexcept { return (int) read(fd, dest, (size_t) length); }

        int doWrite(int fd, const char *data, int length) noexcept { return (int) write(fd, data, (size_t) length); }
    };

    namespace test
    {
        struct TestData
        {
            int i;
            char tab[1024];
            int j;
        };

        int tcpConnection()
        {
            putils::TCPConnection   sock("127.0.0.1", 4242);

            // Receive string
            std::cout << sock.receive() << std::endl;

            TestData data {1, "x\0", 2 };

            // Send structure
            sock.send(data);

            // Receive structure
            sock.receive(data);
            std::cout << data.i << std::endl;
            std::cout << data.tab << std::endl;
            std::cout << data.j << std::endl;

            // Send string
            sock.send("Pipi caca prout\n");

            return 0;
        }
    }
}
