#include <chrono>
#include <thread>
#include <sstream>
#include <fstream>

#include "SecureConnectionModule.hpp"

#include "Packets.hpp"
#include "mediator/Mediator.hpp"
#include "concat.hpp"

SecureConnectionModule::SecureConnectionModule(putils::Mediator &mediator, short normalPort, short securePort)
    : putils::BaseModule(&mediator)
{
    if (normalPort > 0)
        _connections.emplace_back(std::make_unique<putils::TCPListener>(normalPort));
    if (securePort > 0)
        _connections.emplace_back(std::make_unique<putils::OpenSSLTCPListener>(securePort));

    for (auto &connection : _connections)
    {
        try
        {
            connection.connection->addObserver(
                    [this, &connection]()
                    {
                        newConnection(*connection.connection, connection.requests);
                    });

            mediator.runTask(
                    [this, &connection]()
                    {
                        runServer(*connection.connection, connection.mutex, connection.requests);
                    });
        }
        catch (std::runtime_error &e)
        {
            std::cerr << "[SecureConnection] " << e.what() << std::endl;
        }
    }
}

void SecureConnectionModule::runServer(putils::ATCPListener &server, std::mutex &mutex,
                                       const std::atomic<int> &requests) const noexcept
{
    try
    {
        while (getMediator() != nullptr && getMediator()->running == true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::lock_guard<std::mutex> _(mutex);
            if (requests > 0) // If requests are being handled, timeout immediately to see if they're done
                server.select({0, 0});
            else // If no requests are being handled, timeout in 2s to see if server is still running
                server.select({2, 0});
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void SecureConnectionModule::newConnection(putils::ATCPListener &server, std::atomic<int> &requests) const noexcept
{
    auto &client = server.getLastClient();
    client.newMessage += [this, &client, &requests]() { newMessage(requests, client); };
    send(kia::packets::Log{ "[SecureConnection] New client" });
}

void SecureConnectionModule::newMessage(std::atomic<int> &requests, putils::ATCPListener::Client &client) const noexcept
{
    ++requests;
    std::string msg = client.getMsg();

    // Log this message
    send(kia::packets::Log{
            putils::concat("[SecureConnection] Receiving\n",
                    "[", msg, "]\n")
            });

    // Notify other modules of the incoming message
    runTask([this, &client, msg]()
    {
        send(kia::packets::IncomingMessage{ client.fd, msg });
    });
}

void SecureConnectionModule::handle(const kia::packets::OutgoingMessage &packet) noexcept
{

    for (auto &connection : _connections)
    {
        if (connection.connection != nullptr)
        {
            std::lock_guard<std::mutex> _(connection.mutex);
            trySend(packet, *connection.connection, connection.requests);
        }
    }
}

void SecureConnectionModule::trySend(const kia::packets::OutgoingMessage &p, putils::ATCPListener &connection,
                                     std::atomic<int> &requests) const noexcept
{
    if (connection.hasClient(p.clientFd))
    {
        --requests;

        send(kia::packets::Log{
                putils::concat("[SecureConnection] Sending:\n",
                        "[", p.msg, "]\n")
                });

        auto &client = connection.getClient(p.clientFd);
        client.send(p.msg + "\r\n");
        // TODO: this probably shouldn't be here, but up to this point clients need the connection to be closed
        // in order to accept the response
        client.disconnect();
    }
}
