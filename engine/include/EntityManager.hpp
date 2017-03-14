//
// Created by naliwe on 7/14/16.
//

#ifndef KENGINE_ENTITYMANAGER_HPP
# define KENGINE_ENTITYMANAGER_HPP

# include <string>
# include <unordered_map>
# include <memory>
# include <type_traits>
# include <iostream>
# include "GameObject.hpp"
# include "Component.hpp"
# include "ComponentFactory.hpp"
# include "SystemManager.hpp"
# include "EntityFactory.hpp"
# include "ModuleMediator.hpp"

class EntityManager final
{
public:
    EntityManager(std::unique_ptr<EntityFactory> &&factory)
            : _factory(std::move(factory))
    {}

    ~EntityManager() = default;

public:
    EntityManager(EntityManager const& o) = delete;
    EntityManager(EntityManager&& o) = delete;
    EntityManager& operator=(EntityManager const& o) = delete;

public:
    void execute() { _sm.execute(); }

public:
    template<typename T, typename ...Args>
    void createSystem(Args &&...args)
    {
        auto &s = _sm.registerSystem<T>(std::forward<Args>(args)...);
        _mediator.addModule(&s);
    }

public:
    GameObject &createEntity(const std::string &type, const std::string &name)
    {
        return addEntity(name, _factory->make(type, name));
    }

    template<class GO, class ...Args,
             typename = std::enable_if_t<std::is_base_of<GameObject, GO>::value>>
    GO& createEntity(std::string const& name, Args&& ... params) noexcept
    {
        return addEntity(name, std::make_unique<GO>(name, std::forward<Args>(params)...));
    }

private:
    GameObject &addEntity(const std::string &name, std::unique_ptr<GameObject> &&obj)
    {
        auto &ret = *obj;
        const auto p = _entities.emplace(name, std::move(obj));
        _sm.registerGameObject(*p.first->second);
        return ret;
    }

public:
    void removeEntity(std::string const& name)
    {
        const auto p = _entities.find(name);
        if (p == _entities.end())
            throw std::out_of_range("No such entity");

        const auto ret = p->second.get();
        _entities.erase(p);
        _sm.removeGameObject(*ret);
    }

    template<class CT, class ... Args>
    CT& attachComponent(GameObject& parent, std::string const& name, Args&& ... params) noexcept
    {
        auto str = hashCompName(parent.get_name(), name);

        _components.emplace(str,
                ComponentFactory::createComponent<CT>(name, std::forward<Args>(params)...)
        );

        auto &comp = _components[str];
        parent.attachComponent(comp.get());
        _compHierarchy.emplace(name, parent.get_name());

        return static_cast<CT&>(*comp);
    };

    void detachComponent(GameObject& go, Component& comp)
    {
        if (_components.find(hashCompName(go.get_name(), comp.get_name())) == _components.end())
            throw std::logic_error("Could not find component " + comp.toString());
        if (_entities.find(go.get_name()) == _entities.end())
            throw std::logic_error("Could not find entity " + go.get_name());

        //TODO: Recycle component
        _compHierarchy.erase(comp.get_name());
        go.detachComponent(&comp);
        _components.erase(hashCompName(go.get_name(), comp.get_name()));
    }

public:
    void send(const putils::DataPacket &packet) { _mediator.send(packet); }
    void runTask(const std::function<void()> &f) { _mediator.runTask(f); }

private:
    static std::string hashCompName(std::string const& pName, std::string const& cName)
    { return pName + "--" + cName; }

private:
    putils::ModuleMediator _mediator;
    SystemManager _sm;
    std::unique_ptr<EntityFactory> _factory;

private:
    std::unordered_map<std::string, std::unique_ptr<GameObject>> _entities;
    std::unordered_map<std::string, std::unique_ptr<IComponent>>  _components;
    std::unordered_map<std::string, std::string>                 _compHierarchy;
};

#endif //KENGINE_ENTITYMANAGER_HPP
