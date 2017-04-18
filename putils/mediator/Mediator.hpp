#pragma once

#include <atomic>
#include <algorithm>
#include <unordered_map>
#include <future>
#include "Module.hpp"
#include "hash.hpp"

namespace putils
{
    class Mediator
    {
    public:
        ~Mediator() { running = false; completeTasks(); }

    public:
        void addModule(BaseModule *m);
        void removeModule(BaseModule *m);

    public:
        template<typename T>
        void send(T &&data)
        {
            sendDataPacket(DataPacket<T>(std::forward<T>(data)));
        }

    public:
        void sendDataPacket(const ADataPacket &packet);

    public:
        void runTask(const std::function<void()> &f) { _tasks.push_back(std::async(std::launch::async, f)); }

        void completeTasks()
        {
            for (auto &t : _tasks)
                t.get();
            _tasks.clear();
        }

    public:
        std::atomic<bool> running { true };

    private:
        // Map a type to the modules subscribing to that type
        std::unordered_map<pmeta::type_index, std::vector<putils::BaseModule*>> _modules;
        std::vector<std::future<void>> _tasks;
    };
}
