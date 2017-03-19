//
// Created by naliwe on 2/12/17.
//

#pragma once

#include <vector>
#include <memory>
#include "ISystem.hpp"
#include "GameObject.hpp"

namespace kengine
{
    class SystemManager
    {
    public:
        SystemManager() = default;
        ~SystemManager() = default;

    public:
        SystemManager(SystemManager const &o) = delete;
        SystemManager &operator=(SystemManager const &o) = delete;

    public:
        void execute()
        {
            for (auto &category : _systems)
                for (auto &s : category.second)
                    s->execute();
        }

    public:
        void registerGameObject(GameObject &gameObject)
        {
            for (auto &category : _systems)
                if (matchMasks(category.first, gameObject))
                    for (auto &s : category.second)
                        s->registerGameObject(gameObject);
        }

        void removeGameObject(GameObject &gameObject)
        {
            for (auto &category : _systems)
                if (matchMasks(category.first, gameObject))
                    for (auto &s : category.second)
                        s->removeGameObject(gameObject);
        }

    private:
        bool matchMasks(pmeta::type_index mask, const kengine::GameObject &go)
        {
            const auto &goMask = go.getTypes();
            return std::find(goMask.begin(), goMask.end(), mask) != goMask.end();
        }

    public:
        template<typename T, typename ...Args,
                typename = std::enable_if_t<std::is_base_of<ISystem, T>::value>>
        T &registerSystem(Args &&...args)
        {
            auto system = std::make_unique<T>(std::forward<Args>(args)...);
            auto &ret = *system;
            auto &category = _systems[ret.getCompType()];
            category.emplace_back(std::move(system));
            return ret;
        }

    public:
        void registerSystem(std::unique_ptr<ISystem> &&system)
        {
            auto &category = _systems[system->getCompType()];
            category.emplace_back(std::move(system));
        }

    private:
        using Category = std::vector<std::unique_ptr<ISystem>>;
        std::unordered_map<pmeta::type_index, Category> _systems;
    };
}
