#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <cstring>

#include <vector>
#include <algorithm>
#include <unistd.h>
#include "RAII.hpp"
#include "Observable.hpp"

#include "fwd.hpp"

namespace putils
{
    class ATCPListener : public Observable<>
    {
    public:
        struct Client
        {
            Client(int fd, auto &&addr, short port, ATCPListener &connection)
                    : fd(fd, [](int fd) { close(fd); }),
                      addr(FWD(addr)),
                      port(port),
                      _connection(connection)
            {}

            Client(putils::RAII<int> &&fd, auto &&addr, short port, ATCPListener &connection)
                    : fd(std::move(fd)),
                      addr(FWD(addr)),
                      port(port),
                      _connection(connection)
            {}

            putils::Observable<> newMessage;
            putils::Observable<> disconnected;

            putils::RAII<int> fd;
            std::string addr;
            short port;

            template<typename T>
            T getBuff() const { return reinterpret_cast<const T>(*(_buff.get())); }

            std::string getMsg() const { return std::string(_buff.get()); }

            template<typename T>
            void send(T &&data) { _connection.send(*this, FWD(data)); }

            template<typename T = std::string>
            T receive(bool selectAlreadyRunning = false, timeval timeout = { -1, -1 })
            {
                return _connection.receive<T>(*this, selectAlreadyRunning, timeout);
            }

            void disconnect() const { _connection.removeClient(fd); }

        public:
/*        public:
            auto addObserver(const std::function<void()> &func)
            {
                std::lock_guard<std::mutex> _(_mutex);
                return Observable::addObserver(func);
            }

            void removeObserver(intptr_t id)
            {
                std::lock_guard<std::mutex> _(_mutex);
                return Observable::removeObserver(id);
            }*/

        private:
            friend ATCPListener;

            void setBuff(std::unique_ptr<char[]> &&buff)
            {
                _buff = std::move(buff);
            }

        private:
            mutable std::mutex _mutex;
            ATCPListener &_connection;
            std::unique_ptr<char[]> _buff;
        };

    protected:
        virtual void doRemove(int fd) noexcept = 0;

        // Any handshake process (such as SSL_accept) is done here
        virtual bool doAccept(int fd) noexcept = 0;

        virtual int doRead(int fd, char *dest, size_t length) noexcept = 0;

        virtual int doWrite(int fd, const char *data, size_t length) noexcept = 0;

    public:
        virtual ~ATCPListener() = default;

    public:
        ATCPListener(short port, const std::string &host = "127.0.0.1", bool verbose = false)
                : _verbose(verbose)
        {
            FD_ZERO(&_all_fds);

            // Open socket
            _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (_serverSocket < 0)
                throw std::runtime_error("Failed to create socket");
            _maxfd = _serverSocket + 1;
            FD_SET(_serverSocket, &_all_fds);

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(host.c_str());
            addr.sin_port = htons(port);
            if (bind(_serverSocket, (struct sockaddr *) (&addr), sizeof(addr)) != 0)
                throw std::runtime_error("Failed to bind");
            if (listen(_serverSocket, 128) != 0)
                throw std::runtime_error("Failed to listen");
        }

    public:
        void select(timeval &&timeout = {-1, -1})
        {
            fd_set read_fds = _all_fds;
            fd_set write_fds;
            FD_ZERO(&write_fds);

            {
                std::lock_guard<std::recursive_mutex> _(_clientsMutex);
                for (const auto &p : _toWrite)
                    FD_SET(p.first, &write_fds);
            }

            int ret;
            if (timeout.tv_sec != -1 || timeout.tv_usec != -1)
                ret = ::select(_maxfd, &read_fds, &write_fds, nullptr, &timeout);
            else
                ret = ::select(_maxfd, &read_fds, &write_fds, nullptr, nullptr);

            if (ret < 0)
            {
                perror("select");
                throw std::runtime_error("Failed to select");
            }

            if (FD_ISSET(_serverSocket, &read_fds))
                acceptClient();

            std::lock_guard<std::recursive_mutex> _(_clientsMutex);
            for (const auto &c : _clients)
            {
                int fd = c->fd;
                if (FD_ISSET(fd, &read_fds))
                    readFromClient(fd);
                if (FD_ISSET(fd, &write_fds))
                {
                    const auto &p = _toWrite.at(fd);
                    writeToClient(fd, p.first.get(), p.second);
                    _toWrite.erase(fd);
                }
            }

            std::vector<int> erasing;
            for (auto fd : _toRemove)
            {
                if (_toWrite.find(fd) == _toWrite.end())
                {
                    reallyRemoveClient(fd);
                    erasing.push_back(fd);
                }
            }
            for (auto fd : erasing)
                _toRemove.erase(fd);

            for (const auto &c : _newMsgs)
                c->newMessage();
            _newMsgs.clear();
        }

    private:
        template<typename T>
        void send(Client &client, const T &msg)
        {
            std::lock_guard<std::mutex> _(client._mutex);

            auto p = std::make_pair(std::unique_ptr<char[]>(new char[sizeof(T)]), sizeof(T));
            std::memcpy(p.first.get(), &msg, sizeof(T));
            _toWrite.emplace(client.fd, std::move(p));
        }

    private:
        template<typename T = std::string>
        T receive(Client &client, bool selectAlreadyRunning = false, timeval timeout = { -1, -1 })
        {
            int fd = client.fd;

            std::atomic<bool> done { false };
            T ret;

            const auto id = client.newMessage.addObserver([&client, &done, &ret]
            {
                ret = client.getBuff<T>();
                done = true;
            });

            const auto rmid = client.disconnected.addObserver([&done] { done = true; });

            while (!done)
                if (!selectAlreadyRunning)
                    select(timeval(timeout));

            if (std::find_if(_clients.begin(), _clients.end(), [&client](const auto &c) { return c.get() == &client; })
                    == _clients.end() || _toRemove.find(fd) != _toRemove.end())
                throw std::runtime_error("Client disconnected");

            client.newMessage.removeObserver(id);
            client.disconnected.removeObserver(id);

            return ret;
        }

    public:
        Client &getLastClient()
        {
            std::lock_guard<std::recursive_mutex> _(_clientsMutex);
            return *_clients.back();
        }

        void removeClient(int fd)
        {
            if (_toWrite.find(fd) == _toWrite.end())
            {
                std::lock_guard<std::recursive_mutex> _(_clientsMutex);
                reallyRemoveClient(fd);
            }
            else
                _toRemove.emplace(fd);
            // const auto it = _toWrite.find(fd);
            // if (it != _toWrite.end())
                // _toWrite.erase(it);
        }

        bool hasClient(int fd)
        {
            return std::find_if(_clients.begin(), _clients.end(), [fd](const auto &c)
                    { return c->fd == fd; }
                    ) != _clients.end();
        }

        const Client &getClient(int fd) const
        {
            auto it = std::find_if(_clients.cbegin(), _clients.cend(), [fd](const auto &c) { return c->fd == fd; });
            if (it == _clients.end())
                throw std::runtime_error("No such client");
            return **it;
        }

        Client &getClient(int fd)
        {
            auto it = std::find_if(_clients.begin(), _clients.end(), [fd](const auto &c) { return c->fd == fd; });
            if (it == _clients.end())
                throw std::runtime_error("No such client");
            return **it;
        }


    private:
        void reallyRemoveClient(int fd)
        {
            if (!hasClient(fd))
            {
                _toWrite.erase(fd);
                _toRemove.erase(fd);
                return;
            }

            auto &client = getClient(fd);

            {
                std::lock_guard<std::mutex> _(client._mutex);
                doRemove(fd);
            }

            FD_CLR(fd, &_all_fds);
            auto it = std::find_if(_clients.begin(), _clients.end(),
                    [fd](const auto &c) { return c->fd == fd; });

            if (it != _clients.end())
            {
                {
                    std::lock_guard<std::mutex> _(client._mutex);
                    (*it)->disconnected();
                    close((*it)->fd);
                }
                _clients.erase(it);
            }

            _toWrite.erase(fd);
        }

    private:
        void addClient(std::unique_ptr<Client> &&client) noexcept
        {
            std::lock_guard<std::recursive_mutex> _(_clientsMutex);
            _clients.push_back(std::move(client));
        }

    private:
        void acceptClient() noexcept
        {
            sockaddr_in addr;
            socklen_t size = sizeof(addr);
            auto fd = putils::RAII<int>(accept(_serverSocket, (sockaddr *) &addr, &size),
                    [](int fd) { close(fd); });

            if (fd < 0)
                return;
            if (doAccept(fd) == false)
                return;

            int rfd = fd;
            FD_SET(rfd, &_all_fds);
            if (_maxfd <= rfd)
                _maxfd = rfd + 1;

            addClient(std::make_unique<Client>(
                    std::move(fd), inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), *this
            ));

            // Notify observers a new client arrived
            changed();
        }

        void writeToClient(int fd, const char *msg, size_t length) noexcept
        {
            auto &client = getClient(fd);

            std::lock_guard<std::mutex> _(client._mutex);

            size_t written = 0;
            while (written < length)
            {
                auto ret = doWrite(fd, msg + written, length - written);
                if (ret < 0)
                {
                    _toRemove.emplace(fd);
                    _toWrite.erase(fd);
                    return;
                }
                written += ret;
            }
        }

        void readFromClient(int fd)
        {
            try
            {
                char buff[1024];
                std::unique_ptr<char[]> totalBuff = nullptr;
                size_t totalLength = 0;
                auto &client = getClient(fd);

                int ret;
                {
                    std::lock_guard<std::mutex> _(client._mutex);
                    while ((ret = doRead(fd, buff, sizeof(buff))) == sizeof(buff))
                    {
                        totalLength += sizeof(buff);
                        std::unique_ptr<char[]> tmp(new char[totalLength]);
                        if (totalBuff != nullptr)
                            std::memcpy(tmp.get(), totalBuff.get(), totalLength - sizeof(buff));
                        std::memcpy(tmp.get() + totalLength - sizeof(buff), buff, ret);
                        std::memset(tmp.get() + totalLength - sizeof(buff) + ret, 0, sizeof(buff) - ret);
                        totalBuff = std::move(tmp);
                    }
                }

                if (ret > 0)
                {
                    totalLength += sizeof(buff);
                    std::unique_ptr<char[]> tmp(new char[totalLength]);
                    if (totalBuff != nullptr)
                        std::memcpy(tmp.get(), totalBuff.get(), totalLength - sizeof(buff));
                    std::memcpy(tmp.get() + totalLength - sizeof(buff), buff, ret);
                    std::memset(tmp.get() + totalLength - sizeof(buff) + ret, 0, sizeof(buff) - ret);
                    totalBuff = std::move(tmp);
                }
                    // Client disconnected, or an error occured and I say fuck'em
                else //if (ret == 0)
                {
                    removeClient(fd);
                    return;
                }

                {
                    std::lock_guard<std::mutex> _(client._mutex);
                    client.setBuff(std::move(totalBuff));
                }

                _newMsgs.push_back(&client);
            }
            catch (std::out_of_range &e)
            {
                std::cerr << "Tried to write to non-existing client" << std::endl;
            }
        }

    protected:
        bool isVerbose() const { return _verbose; }

    private:
        bool _verbose;
        std::recursive_mutex _clientsMutex;
        std::vector<std::unique_ptr<Client>> _clients;
        std::vector<Client*> _newMsgs;

    private:
        fd_set _all_fds;
        int _maxfd;
        putils::RAII<int> _serverSocket{0, [](int sock) { close(sock); }};

    private:
        std::unordered_map<int, std::pair<std::unique_ptr<char[]>, size_t>> _toWrite;
        std::unordered_set<int> _toRemove;
    };

    template<>
    inline std::string ATCPListener::Client::getBuff() const { return getMsg(); }

    template<>
    inline void ATCPListener::send(Client &client, const std::string &msg)
    {
        std::lock_guard<std::mutex> _(client._mutex);

        auto p = std::make_pair(std::unique_ptr<char[]>(new char[msg.length()]), msg.length());
        std::memcpy(p.first.get(), msg.c_str(), msg.length());
        _toWrite.emplace(client.fd, std::move(p));
    }
}
