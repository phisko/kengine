//
// Created by naliwe on 7/15/16.
//

#ifndef KENGINE_ISYSTEM_HPP
# define KENGINE_ISYSTEM_HPP

# include <vector>
# include "Component.hpp"
# include "GameObject.hpp"
# include "Module.hpp"

namespace kengine
{
    class ISystem : public putils::Module
    {
    protected:
        ISystem(int tag = putils::ADataPacket::Ignore) : Module(tag) {}

    public:
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
        virtual ComponentMask getMask() const noexcept { return Mask; }

    protected:
        using ISystem::ISystem;
        auto &getGameObjects() { return _gameObjects; }
        auto &getGameObjects() const { return _gameObjects; }

    public:
        void registerGameObject(GameObject &go) override
        {
            const auto it = std::find(_gameObjects.begin(), _gameObjects.end(), &go);
            if (it == _gameObjects.end())
                _gameObjects.push_back(&go);
        }
        void removeGameObject(GameObject &go) override
        {
            _gameObjects.erase(std::find(_gameObjects.begin(), _gameObjects.end(), &go));
        }

    private:
        std::vector<GameObject*> _gameObjects;
    };
}

#endif //KENGINE_ISYSTEM_HPP
