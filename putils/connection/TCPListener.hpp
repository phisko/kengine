#pragma once

#include "ATCPListener.hpp"

namespace putils
{
    class TCPListener final : public ATCPListener
    {
    public:
        TCPListener(short port, const std::string &host = "127.0.0.1", bool verbose = false)
                : ATCPListener(port, host, verbose) {}

    private:
        void doRemove(int) noexcept {}

        bool doAccept(int) noexcept { return true; }

        int doRead(int fd, char *dest, size_t length) noexcept { return (int) read(fd, dest, length); }

        int doWrite(int fd, const char *data, size_t length) noexcept { return (int) write(fd, data, length); }
    };

    namespace test
    {
        struct TestData
        {
            int i;
            char tab[1024];
            int j;
        };

        inline int tcpListener()
        {
            // Listen on port 4242
            putils::TCPListener server(4242);

            // Observe server for new clients
            server.addObserver([&server]()
            {
                auto &client = server.getLastClient();
                // Capture client's fd as the client reference may be invalidated

                client.send("Welcome!\n");

                int first = 0;
                // Observe client for new messages
                client.newMessage += [&server, &client, &first]()
                {
                    // On first message, receive structure
/*                    if (first == 0)
                    {
                        // Get message from client
                        std::cout << server.getClient(fd).getBuff<TestData>().i << std::endl;
                        std::cout << server.getClient(fd).getBuff<TestData>().tab << std::endl;
                        std::cout << server.getClient(fd).getBuff<TestData>().j << std::endl;

                        TestData out { 42, "sending this back\0", 24 };
                        // Send message to client
                        // server.send(fd, "Hello there!\n");
                        server.send(fd, out);
                        first = 1;
                    }
                        // On second, receive string
                    else
                    {
 */                       std::cout << client.getMsg() << std::endl;
                        first = 0;
                    // }
                };
            });

            while (true)
                server.select();
        }
    }
}