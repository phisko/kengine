//
// Created by naliwe on 2/12/17.
//

#pragma once

#include <vector>
#include <memory>
#include "ISystem.hpp"
#include "GameObject.hpp"
#include "Mediator.hpp"

namespace kengine
{
    class SystemManager : public putils::Mediator
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
        template<typename T, typename ...Args>
        void createSystem(Args &&...args)
        {
            if constexpr (!std::is_base_of<ISystem, T>::value)
                static_assert("Attempt to create something that's not a System");

            auto system = std::make_unique<T>(std::forward<Args>(args)...);
            auto module = system.get();
            auto &category = _systems[module->getCompType()];
            category.emplace_back(std::move(system));
            addModule(module);
        }

        void addSystem(std::unique_ptr<ISystem> &&system)
        {
            addModule(system.get());
            auto &category = _systems[system->getCompType()];
            category.emplace_back(std::move(system));
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

        // Implementation detail
    private:
        bool matchMasks(pmeta::type_index mask, const kengine::GameObject &go)
        {
            const auto &goMask = go.getTypes();
            return std::find(goMask.begin(), goMask.end(), mask) != goMask.end();
        }

    private:
        using Category = std::vector<std::unique_ptr<ISystem>>;
        std::unordered_map<pmeta::type_index, Category> _systems;
    };
}
