#pragma once

#include <string_view>
#include <string>
#include <unordered_map>
#include <memory>
#include <type_traits>
#include <iostream>
#include "SystemManager.hpp"
#include "ComponentManager.hpp"
#include "EntityFactory.hpp"

namespace kengine
{
    class EntityManager : public SystemManager, public ComponentManager
    {
    public:
        EntityManager(std::unique_ptr<EntityFactory>&& factory = std::make_unique<ExtensibleFactory>())
                : _factory(std::move(factory))
        {}

        ~EntityManager() = default;

    public:
        GameObject& createEntity(std::string_view type, std::string_view name,
                                 const std::function<void(GameObject&)>& postCreate = nullptr)
        {
            auto e = _factory->make(type, name);

            if (postCreate != nullptr)
                postCreate(*e);

            return addEntity(name, std::move(e));
        }

        template<class GO, typename ...Params>
        GO &createEntity(std::string_view name,
                         const std::function<void(GameObject &)> &postCreate = nullptr,
                         Params &&... params)
        {
            static_assert(std::is_base_of<GameObject, GO>::value,
                          "Attempt to create something that's not a GameObject");

            auto entity = std::make_unique<GO>(name, FWD(params)...);

            if (postCreate != nullptr)
                postCreate(static_cast<GameObject&>(*entity));

            return static_cast<GO&>(addEntity(name, std::move(entity)));
        }

    private:
        GameObject& addEntity(std::string_view name, std::unique_ptr<GameObject>&& obj)
        {
            auto& ret = *obj;

            if (_entities.find(name.data()) != _entities.end())
                throw std::runtime_error("Entity exists");

            _entities[name.data()] = std::move(obj);

            ComponentManager::registerGameObject(ret);
            SystemManager::registerGameObject(ret);

            return ret;
        }

    public:
        void removeEntity(kengine::GameObject& go)
        {
            SystemManager::removeGameObject(go);
            ComponentManager::removeGameObject(go);
            _entities.erase(_entities.find(go.getName().data()));
        }

        void removeEntity(std::string_view name)
        {
            const auto p = _entities.find(name.data());
            if (p == _entities.end())
                throw std::out_of_range("No such entity");

            const auto &
            [_, e] = *p;

            SystemManager::removeGameObject(*e);
            ComponentManager::removeGameObject(*e);
            _entities.erase(p);
        }

    public:
        GameObject& getEntity(std::string_view name)
        { return *_entities.at(name.data()); }

        bool hasEntity(std::string_view name) const noexcept
        { return _entities.find(name.data()) != _entities.end(); }

    public:
        void addLink(const GameObject& parent, const GameObject& child)
        { _entityHierarchy[&child] = &parent; }

        void removeLink(const GameObject& child)
        { _entityHierarchy.erase(&child); }

        const GameObject& getParent(const GameObject& go) const
        { return *_entityHierarchy.at(&go); }

    public:
        template<typename T>
        T& getFactory()
        { return static_cast<T&>(*_factory); }

        template<typename T>
        const T& getFactory() const
        { return static_cast<const T&>(*_factory); }

    public:
        template<typename RegisterWith, typename ...Types>
        void registerTypes()
        {
            if constexpr (!std::is_same<RegisterWith, nullptr_t>::value)
            {
                try
                {
                    auto &s = getSystem<RegisterWith>();
                    s.template registerTypes<Types...>();
                }
                catch (const std::out_of_range &) {}
            }

            try
            {
                auto &factory = getFactory<kengine::ExtensibleFactory>();
                pmeta::tuple_for_each(std::make_tuple(pmeta::type<Types>()...),
                                      [&factory](auto &&t)
                                      {
                                          using Type = pmeta_wrapped(t);
                                          if constexpr (std::is_base_of<kengine::GameObject, Type>::value)
                                              factory.registerType<Type>();
                                      }
                );
            }
            catch (const std::out_of_range &) {}
        }

    private:
        std::unique_ptr<EntityFactory> _factory;

    private:
        std::unordered_map<std::string, std::unique_ptr<GameObject>> _entities;
        std::unordered_map<const GameObject*, const GameObject*>     _entityHierarchy;
    };
}
