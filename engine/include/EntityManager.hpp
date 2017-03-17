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

namespace kengine
{
    class EntityManager final
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
            _mediator.addModule(&s);
        }

    public:
        GameObject &createEntity(const std::string &type, const std::string &name)
        {
            return addEntity(name, _factory->make(type, name));
        }

        template<class GO, class ...Args,
                typename = std::enable_if_t<std::is_base_of<GameObject, GO>::value>>
        GO &createEntity(std::string const &name, Args &&... params) noexcept
        {
            auto entity = std::make_unique<GO>(name, std::forward<Args>(params)...);

            for (const auto &p : entity->_components)
                registerComponent(*entity, std::unique_ptr<IComponent>(p.second));

            return addEntity(name, std::move(entity));
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
            const auto &name = comp->get_name();
            const auto &parentName = parent.get_name();
            const auto str = hashCompName(parentName, name);

            _components.emplace(str, std::move(comp));
            _compHierarchy.emplace(name, parentName);
        }

    public:
        void detachComponent(GameObject &go, IComponent &comp)
        {
            if (_components.find(hashCompName(go.get_name(), comp.get_name())) == _components.end())
                throw std::logic_error("Could not find component " + comp.toString());
            if (_entities.find(go.get_name()) == _entities.end())
                throw std::logic_error("Could not find entity " + go.get_name());

            //TODO: Recycle component
            _compHierarchy.erase(comp.get_name());
            go.detachComponent(&comp);
            _components.erase(hashCompName(go.get_name(), comp.get_name()));

            // TODO: find some way to call removeGameObject on systems
        }

    public:
        void send(const putils::ADataPacket &packet) { _mediator.send(packet); }

        void runTask(const std::function<void()> &f) { _mediator.runTask(f); }

    private:
        static std::string hashCompName(std::string const &pName, std::string const &cName)
        {
            return pName + "--" + cName;
        }

    private:
        putils::ModuleMediator _mediator;
        SystemManager _sm;
        std::unique_ptr<EntityFactory> _factory;

    private:
        std::unordered_map<std::string, std::unique_ptr<GameObject>> _entities;
        std::unordered_map<std::string, std::unique_ptr<IComponent>> _components;
        std::unordered_map<std::string, std::string> _compHierarchy;
    };
}

#endif //KENGINE_ENTITYMANAGER_HPP
