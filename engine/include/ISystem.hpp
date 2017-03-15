//
// Created by naliwe on 7/15/16.
//

#ifndef KENGINE_ISYSTEM_HPP
# define KENGINE_ISYSTEM_HPP

#include <KoadomWars.hpp>
# include "Component.hpp"
# include "GameObject.hpp"
# include "Module.hpp"

class ISystem : public putils::Module
{
public:
    static const ComponentMask Mask;

public:
    ISystem(int tag = putils::ADataPacket::Ignore) : Module(tag) {}
    virtual ~ISystem() = default;

public:
    virtual void execute()                          = 0;
    virtual void registerGameObject(GameObject& go) = 0;
    virtual void removeGameObject(GameObject& go) = 0;

protected:
    void log(const std::string &msg) const { send((int)kdw::DataPacketTags::Log, msg); }
};


#endif //KENGINE_ISYSTEM_HPP
