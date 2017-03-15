//
// Created by naliwe on 7/15/16.
//

#ifndef KENGINE_ISYSTEM_HPP
# define KENGINE_ISYSTEM_HPP

# include "Component.hpp"
# include "GameObject.hpp"
# include "Module.hpp"

namespace kengine
{
    class ISystem : public putils::Module
    {
    public:
        ISystem(int tag = putils::ADataPacket::Ignore) : Module(tag) {}
        virtual ~ISystem() = default;

    public:
        virtual void execute()                          = 0;
        virtual void registerGameObject(GameObject& go) = 0;
        virtual void removeGameObject(GameObject& go) = 0;
        virtual ComponentMask getMask() const noexcept = 0;
    };

    template<ComponentMask Mask>
    class System : public ISystem
    {
    public:
        using ISystem::ISystem;
        virtual ComponentMask getMask() const noexcept { return Mask; }
    };
}

#endif //KENGINE_ISYSTEM_HPP
