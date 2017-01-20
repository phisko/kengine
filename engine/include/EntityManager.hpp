//
// Created by naliwe on 7/14/16.
//

#ifndef KENGINE_ENTITYMANAGER_HPP
# define KENGINE_ENTITYMANAGER_HPP

# include <string>
# include <map>
# include <memory>
# include <type_traits>
# include <iostream>
# include "GameObject.hpp"
# include "Component.hpp"
# include "ComponentFactory.hpp"

class EntityManager final
{
 public:
  EntityManager();
  ~EntityManager();

 public:
  EntityManager(EntityManager const& o) = delete;
  EntityManager(EntityManager&& o) = delete;
  EntityManager& operator=(EntityManager const& o) = delete;

 public:
  template<class GO, class ...Args,
           typename = typename std::enable_if<
             std::is_base_of<GameObject, GO>::value
           >::type>
  GO& createEntity(std::string const& name, Args&& ... params) noexcept
  {
    _entities.emplace(name, std::make_unique<GO>(name, std::forward(params)...));

    return *_entities[name];
  }

  template<class CT, class ... Args>
  CT& attachComponent(GameObject& parent, std::string const& name, Args... params) noexcept
  {
    auto str = hashCompName(parent.get_name(), name);

    _components.emplace(str,
                        ComponentFactory::createComponent<CT>(name, std::forward(params)...));

    parent.attachComponent(_components[str].get());
    _compHierarchy.emplace(name, parent.get_name());

    return *_components[str];
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

 private:
  std::string hashCompName(std::string const& pName, std::string const& cName)
  { return pName + "--" + cName; }

 private:
  std::map<std::string, std::unique_ptr<GameObject>> _entities;
  std::map<std::string, std::unique_ptr<Component>>  _components;
  std::map<std::string, std::string>                 _compHierarchy;
};

#endif //KENGINE_ENTITYMANAGER_HPP
