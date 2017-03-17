#pragma once

#include <cstddef>
#include "ISystem.hpp"

namespace kengine
{
    using IgnoreComponents = nullptr_t;

    template<typename RegisteredComponent = IgnoreComponents, typename ...DataPackets>
    class System : public ISystem, public putils::Module<DataPackets...>
    {
    public:
        virtual std::size_t getMask() const noexcept { return pmeta::type<RegisteredComponent>::index; }

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