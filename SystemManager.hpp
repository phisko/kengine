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
            for (auto & [type, systems] : _systems)
                for (auto &s : systems)
                    s->execute();
        }

    public:
        void registerGameObject(GameObject &gameObject) noexcept
        {
            for (auto & [type, systems] : _systems)
                if (matchMasks(type, gameObject))
                    for (auto &s : systems)
                    {
                        try
                        {
                            s->registerGameObject(gameObject);
                        }
                        catch (const std::exception &e) { std::cerr << e.what() << std::endl; }
                    }
        }

        void removeGameObject(GameObject &gameObject)
        {
            for (auto & [type, systems] : _systems)
                if (matchMasks(type, gameObject))
                    for (auto &s : systems)
                    {
                        try
                        {
                            s->removeGameObject(gameObject);
                        }
                        catch (const std::exception &e) { std::cerr << e.what() << std::endl; }
                    }
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
