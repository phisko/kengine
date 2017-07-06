//
// Created by naliwe on 7/14/16.
//

#pragma once

#include <string_view>
#include <string>
#include <unordered_map>
#include <memory>
#include <type_traits>
#include <iostream>
#include "GameObject.hpp"
#include "Component.hpp"
#include "SystemManager.hpp"
#include "EntityFactory.hpp"
#include "Mediator.hpp"

namespace kengine
{
    class EntityManager : public putils::Mediator
    {
    public:
        EntityManager(std::unique_ptr<EntityFactory> &&factory = nullptr)
                : _factory(std::move(factory)) {}

        ~EntityManager() = default;

    public:
        EntityManager(EntityManager const &o) = delete;

        EntityManager(EntityManager &&o) = delete;

        EntityManager &operator=(EntityManager const &o) = delete;

    public:
        void execute() { _sm.execute(); }

    public:
        template<typename T, typename ...Args>
        void createSystem(Args &&...args)
        {
            auto &s = _sm.registerSystem<T>(FWD(args)...);
            addModule(&s);
        }

    public:
        void addSystem(std::unique_ptr<ISystem> &&system)
        {
            addModule(system.get());
            _sm.registerSystem(std::move(system));
        }

    public:
        GameObject &createEntity(std::string_view type, std::string_view name,
                                 const std::function<void(GameObject &)> &postCreate = nullptr)
        {
            auto e = _factory->make(type, name);

            if (postCreate != nullptr)
                postCreate(*e);

            for (const auto & [type, comp] : e->_components)
                registerComponent(*e, *comp);

            return addEntity(name, std::move(e));
        }

        template<class GO, typename = std::enable_if_t<std::is_base_of<GameObject, GO>::value>, typename ...Params>
        GO &createEntity(std::string_view name,
                         const std::function<void(GameObject &)> &postCreate = nullptr,
                         Params &&... params) noexcept
        {
            auto entity = std::make_unique<GO>(name, FWD(params)...);

            if (postCreate != nullptr)
                postCreate(static_cast<GameObject &>(*entity));

            for (const auto & [type, comp] : entity->_components)
                registerComponent(*entity, *comp);

            return static_cast<GO &>(addEntity(name, std::move(entity)));
        }

    private:
        GameObject &addEntity(std::string_view name, std::unique_ptr<GameObject> &&obj)
        {
            auto &ret = *obj;

            if (_entities.find(name.data()) != _entities.end())
                throw std::runtime_error("Entity exists");

            _entities[name.data()] = std::move(obj);
            _sm.registerGameObject(ret);

            return ret;
        }

    public:
        void removeEntity(kengine::GameObject &go)
        {
            _sm.removeGameObject(go);
            _entities.erase(_entities.find(go.getName().data()));
        }

        void removeEntity(std::string_view name)
        {
            const auto p = _entities.find(name.data());
            if (p == _entities.end())
                throw std::out_of_range("No such entity");

            const auto & [_, e] = *p;

            _sm.removeGameObject(*e);
            _entities.erase(p);
        }

    public:
        GameObject &getEntity(std::string_view name)
        {
            const auto it = _entities.find(name.data());
            if (it == _entities.end())
                throw std::out_of_range(putils::concat(name.data(), ": No such entity"));
            return *it->second;
        }

    public:
        bool hasEntity(std::string_view name) const noexcept { return _entities.find(name.data()) != _entities.end(); }

    public:
        template<class CT, typename ...Params>
        CT &attachComponent(GameObject &parent, Params &&... params) noexcept
        {
            auto ptr = std::make_unique<CT>(FWD(params)...);
            auto &comp = *ptr;

            registerComponent(parent, comp);

            parent.attachComponent(std::move(ptr));
            _sm.registerGameObject(parent);

            return comp;
        };

    private:
        void registerComponent(const GameObject &parent, const IComponent &comp)
        {
            _compHierarchy.emplace(&comp, &parent);
        }

    public:
        void detachComponent(GameObject &go, const IComponent &comp)
        {
            if (_entities.find(go.getName().data()) == _entities.end())
                throw std::logic_error(putils::concat("Could not find entity ", go.getName()));

            //TODO: Recycle component
            _compHierarchy.erase(&comp);
            go.detachComponent(comp);

            // TODO: find some way to call removeGameObject on systems
        }

    private:
        SystemManager _sm;
        std::unique_ptr<EntityFactory> _factory;

    private:
        std::unordered_map<std::string, std::unique_ptr<GameObject>> _entities;
        std::unordered_map<const IComponent *, const GameObject *> _compHierarchy;
    };
}
