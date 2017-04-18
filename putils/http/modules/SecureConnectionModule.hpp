#pragma once

#include "mediator/Module.hpp"
#include "connection/OpenSSLTCPListener.hpp"
#include "connection/TCPListener.hpp"

#include "Packets.hpp"

class SecureConnectionModule final : public putils::Module<SecureConnectionModule,
    kia::packets::OutgoingMessage>
{
public:
    SecureConnectionModule(putils::Mediator &mediator, short normalPort = 4242, short securePort = 4243);

public:
    void handle(const kia::packets::OutgoingMessage &packet) noexcept;

private:
    void newConnection(putils::ATCPListener &server, std::atomic<int> &requests) const noexcept;

    void newMessage(std::atomic<int> &requests, putils::ATCPListener::Client &client) const noexcept;

private:
    void trySend(const kia::packets::OutgoingMessage &p,
            putils::ATCPListener &connection,
            std::atomic<int> &requests) const noexcept;

    void runServer(putils::ATCPListener &server, std::mutex &mutex, const std::atomic<int> &requests) const noexcept;

private:
    struct Connection
    {
        Connection(std::unique_ptr<putils::ATCPListener> &&c)
                : connection(std::move(c)), requests(0)
        {}

        Connection(Connection &&other) noexcept : connection(std::move(other.connection)), requests(0) {}

        std::unique_ptr<putils::ATCPListener> connection;
        std::atomic<int> requests;
        std::mutex mutex;
    };

private:
    std::vector<Connection> _connections;
};
