#pragma once

#include <vector>
#include <algorithm>
#include <unistd.h>
#include "putils/RAII.hpp"
#include "putils/Observable.hpp"

namespace putils
{
// Notifies observers when a new client is added
// Get that client through getLastClient()
    class ITCPListener : public Observable<>
    {
    public:
        struct Client : public Observable<>
        {
            template<typename String>
            Client(int fd, String &&addr, short port)
                    : fd(fd, [](int fd) { close(fd); }),
                      addr(std::forward<String>(addr)),
                      port(port) {}

            template<typename String>
            Client(putils::RAII<int> &&fd, String &&addr, short port)
                    : fd(std::move(fd)),
                      addr(std::forward<String>(addr)),
                      port(port) {}

            putils::RAII<int> fd;
            std::string addr;
            short port;

            template<typename String>
            void setBuff(String &&str)
            {
                buff = std::forward<String>(str);
                changed();
            }

            const std::string &getBuff() const { return buff; }

        private:
            std::string buff;
        };

    public:
        virtual ~ITCPListener() = default;

    public:
        // Checks for new clients and reads any incoming message
        // Observe the clients to be notified about those messages
        virtual void select(timeval &&timeout = {-1, -1}) = 0;

        virtual void send(int fd, const std::string &msg) = 0;

        // Client getters and setters
    public:
        virtual Client &getLastClient() = 0;

        virtual bool hasClient(int fd) noexcept = 0;

        virtual void removeClient(int fd) = 0;

        virtual const Client &getClient(int fd) const = 0;
        virtual Client &getClient(int fd) = 0;
    };
}
