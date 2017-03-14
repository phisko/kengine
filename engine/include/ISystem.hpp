//
// Created by naliwe on 7/15/16.
//

#ifndef KENGINE_ISYSTEM_HPP
# define KENGINE_ISYSTEM_HPP

# include "Component.hpp"
# include "GameObject.hpp"
# include "Module.hpp"

class ISystem : public putils::Module
{
public:
    static const ComponentMask Mask;

public:
    ISystem(int tag = putils::DataPacket::Ignore) : Module(tag) {}
    virtual ~ISystem() = default;

public:
    virtual void execute()                          = 0;
    virtual void registerGameObject(GameObject& go) = 0;
    virtual void removeGameObject(GameObject& go) = 0;
};


#endif //KENGINE_ISYSTEM_HPP
