#pragma once

#include <cstddef>
#include "ISystem.hpp"

namespace kengine
{
    using IgnoreComponents = nullptr_t;

    template<typename CRTP, typename RegisteredComponent = IgnoreComponents, typename ...DataPackets>
    class System : public ISystem, public putils::Module<CRTP, DataPackets...>
    {
    protected:
        using ISystem::ISystem;
        std::vector<GameObject*> &getGameObjects() { return _gameObjects; }
        const std::vector<GameObject*> &getGameObjects() const { return _gameObjects; }

    public:
        pmeta::type_index getCompType() const noexcept final
        {
            static_assert(std::is_base_of<System, CRTP>::value, "System's first template parameter should be inheriting class");
            return pmeta::type<RegisteredComponent>::index;
        }

    protected:
        void execute() override {}

        void registerGameObject(GameObject &go) override
        {
            const auto it = std::find(_gameObjects.begin(), _gameObjects.end(), &go);
            if (it == _gameObjects.end())
                _gameObjects.push_back(&go);
        }

        void removeGameObject(GameObject &go) override
        {
            const auto it = std::find(_gameObjects.begin(), _gameObjects.end(), &go);
            if (it != _gameObjects.end())
                _gameObjects.erase(it);
        }

    private:
        std::vector<GameObject*> _gameObjects;
    };
}