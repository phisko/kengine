//
// Created by naliwe on 6/24/16.
//

#pragma once

#include <string>
#include <unordered_map>
#include <algorithm>

#include "IComponent.hpp"
#include "Mediator.hpp"

#include "Serializable.hpp"

#include "concat.hpp"
#include "json.hpp"
#include "to_string.hpp"

namespace kengine
{
    class GameObject : public putils::Mediator, public putils::Serializable<GameObject, false>
    {
    public:
        GameObject(std::string const &name) :
                Serializable(
                        std::make_pair("name", &GameObject::_name),
                        std::make_pair("components", &GameObject::_components)
                ),
                _name(name)
        {}

        GameObject(GameObject &&other) = default;
        GameObject &operator=(GameObject &&other) = default;
        ~GameObject() = default;

    protected:
        friend class EntityManager;

        template<typename CT, typename = std::enable_if_t<std::is_base_of<IComponent, CT>::value>>
        CT &attachComponent(std::unique_ptr<CT> &&comp)
        {
            auto &ret = *comp;

            this->addModule(comp.get());
            const auto type = comp->getType();
            _components[type] = std::move(comp);
            _types.push_back(type);

            return ret;
        }

        void detachComponent(const IComponent &comp);

        template<typename CT, typename ...Args, typename = std::enable_if_t<std::is_base_of<kengine::IComponent, CT>::value>>
        CT &attachComponent(Args &&...args)
        {
            return attachComponent(std::make_unique<CT>(std::forward<Args>(args)...));
        };

    public:
        template<class CT,
                typename = typename std::enable_if_t<
                        std::is_base_of<IComponent, CT>::value>>
        CT &getComponent() const
        {
            const auto it = _components.find(CT::Type);
            if (it == _components.end())
                throw std::out_of_range("Could not find component with provided type");

            return static_cast<CT &>(*(it->second));
        };

    public:
        template<typename CT, typename = std::enable_if_t<std::is_base_of<IComponent, CT>::value>>
        bool hasComponent() const noexcept { return _components.find(CT::Type) != _components.end(); }

    public:
        const std::string &getName() const { return _name; }
        const std::vector<pmeta::type_index> &getTypes() const { return _types; }

    private:
        std::string _name;
        std::unordered_map<pmeta::type_index, std::unique_ptr<IComponent>> _components;
        std::vector<pmeta::type_index > _types;
    };
}
