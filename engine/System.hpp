#pragma once

#include <cstddef>
#include "ISystem.hpp"

namespace kengine
{
    using IgnoreComponents = nullptr_t;

    template<typename RegisteredComponent = IgnoreComponents, typename ...DataPackets>
    class System : public ISystem, public putils::Module<DataPackets...>
    {
    protected:
        using ISystem::ISystem;
        auto &getGameObjects() { return _gameObjects; }
        const auto &getGameObjects() const { return _gameObjects; }

    public:
        pmeta::type_index getCompType() const noexcept override { return pmeta::type<RegisteredComponent>::index; }

    private:
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