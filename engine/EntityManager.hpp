//
// Created by naliwe on 7/14/16.
//

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <type_traits>
#include <iostream>
#include "GameObject.hpp"
#include "Component.hpp"
#include "ComponentFactory.hpp"
#include "SystemManager.hpp"
#include "EntityFactory.hpp"
#include "Mediator.hpp"

namespace kengine
{
    class EntityManager : public putils::Mediator
    {
    public:
        EntityManager(std::unique_ptr<EntityFactory> &&factory)
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
            auto &s = _sm.registerSystem<T>(std::forward<Args>(args)...);
            addModule(&s);
        }

    public:
        void addSystem(std::unique_ptr<ISystem> &&system)
        {
            addModule(system.get());
            _sm.registerSystem(std::move(system));
        }

    public:
        GameObject &createEntity(const std::string &type, const std::string &name,
                                 const std::function<void(GameObject &)> &postCreate = nullptr)
        {
            auto e = _factory->make(type, name);

            if (postCreate != nullptr)
                postCreate(*e);

            for (const auto &p : e->_components)
                registerComponent(*e, std::unique_ptr<IComponent>(p.second));

            return addEntity(name, std::move(e));
        }

        template<class GO, class ...Args,
                typename = std::enable_if_t<std::is_base_of<GameObject, GO>::value>>
        GO &createEntity(std::string const &name,
                         const std::function<void(GameObject &)> &postCreate = nullptr,
                         Args &&... params) noexcept
        {
            auto entity = std::make_unique<GO>(name, std::forward<Args>(params)...);

            if (postCreate != nullptr)
                postCreate(static_cast<GameObject &>(*entity));

            for (const auto &p : entity->_components)
                registerComponent(*entity, std::unique_ptr<IComponent>(p.second));

            return static_cast<GO&>(addEntity(name, std::move(entity)));
        }

    private:
        GameObject &addEntity(const std::string &name, std::unique_ptr<GameObject> &&obj)
        {
            auto &ret = *obj;
            _entities[name] = std::move(obj);
            _sm.registerGameObject(ret);
            // const auto p = _entities.emplace(name, std::move(obj));
            // _sm.registerGameObject(*p.first->second);
            return ret;
        }

    public:
        void removeEntity(std::string const &name)
        {
            const auto p = _entities.find(name);
            if (p == _entities.end())
                throw std::out_of_range("No such entity");

            const auto ret = p->second.get();
            _sm.removeGameObject(*ret);
            _entities.erase(p);
        }

    public:
        GameObject &getEntity(const std::string &name)
        {
            const auto it = _entities.find(name);
            if (it == _entities.end())
                throw std::out_of_range(name + ": No such entity");
            return *it->second;
        }

    public:
        template<class CT, class ... Args>
        CT &attachComponent(GameObject &parent, Args &&... params) noexcept
        {
            auto ptr = ComponentFactory::createComponent<CT>(std::forward<Args>(params)...);
            auto &comp = *ptr;

            registerComponent(parent, std::move(ptr));

            parent.attachComponent(&comp);
            _sm.registerGameObject(parent);

            return static_cast<CT &>(comp);
        };

    private:
        void registerComponent(const GameObject &parent, std::unique_ptr<IComponent> &&comp)
        {
            const auto &parentName = parent.getName();

            _compHierarchy.emplace(comp.get(), parentName);
            _components.push_back(std::move(comp));
        }

    public:
        void detachComponent(GameObject &go, const IComponent &comp)
        {
            const auto it = std::find_if(_components.begin(), _components.end(), [&comp](auto &&ptr) { return ptr.get() == &comp; });

            if (it == _components.end())
                throw std::logic_error("Could not find component " + comp.toString());
            if (_entities.find(go.getName()) == _entities.end())
                throw std::logic_error("Could not find entity " + go.getName());

            //TODO: Recycle component
            _compHierarchy.erase(&comp);
            go.detachComponent(&comp);
            _components.erase(it);

            // TODO: find some way to call removeGameObject on systems
        }

    private:
        static std::string hashCompName(std::string const &pName, std::string const &cName)
        {
            return pName + "--" + cName;
        }

    private:
        SystemManager _sm;
        std::unique_ptr<EntityFactory> _factory;

    private:
        std::unordered_map<std::string, std::unique_ptr<GameObject>> _entities;
        std::vector<std::unique_ptr<IComponent>> _components;
        std::unordered_map<const IComponent *, std::string> _compHierarchy;
    };
}
