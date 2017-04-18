#include "Module.hpp"
#include "Mediator.hpp"

namespace putils
{
    BaseModule::BaseModule(Mediator *mediator)
            : _mediator(mediator)
    {
    }

    BaseModule::~BaseModule()
    {
        _mediator->removeModule(this);
    }

    void BaseModule::sendDataPacket(const ADataPacket &packet) const noexcept
    {
        _mediator->sendDataPacket(packet);
    }

    void BaseModule::sendDataPacketTo(const ADataPacket &packet, BaseModule *dest) const noexcept
    {
        dest->receive(packet);
    }

    void BaseModule::runTask(const std::function<void()> &f) const
    {
        _mediator->runTask(f);
    }
}
