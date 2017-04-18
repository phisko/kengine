#pragma once

#include <vector>
#include <functional>
#include "DataPacket.hpp"
#include "fwd.hpp"

namespace putils
{
    class Mediator;

    class BaseModule
    {
    public:
        BaseModule(Mediator *mediator = nullptr);
        virtual ~BaseModule();

    public:
        virtual const std::vector<pmeta::type_index> &getSubscriptions() const noexcept = 0;
        virtual void receive(const ADataPacket &) noexcept = 0;

    public:
        void setMediator(Mediator *mediator) { _mediator = mediator; }
        const Mediator *getMediator() const { return _mediator; }
        Mediator *getMediator() { return _mediator; }

    protected:
        template<typename T>
        void send(const T &data) const
        {
            sendDataPacket(DataPacket<T>(data));
        }

        template<typename T>
        void sendTo(const T &data, BaseModule *dest) const
        {
            sendDataPacketTo(DataPacket<T>(data), dest);
        }

    private:
        void sendDataPacket(const ADataPacket &packet) const noexcept;
        void sendDataPacketTo(const ADataPacket &packet, BaseModule *dest) const noexcept;

    protected:
        void runTask(const std::function<void()> &f) const;

    private:
        Mediator *_mediator;
    };
}
