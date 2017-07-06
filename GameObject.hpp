#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <algorithm>
#include <memory>

#include "IComponent.hpp"
#include "Mediator.hpp"

#include "Serializable.hpp"

#include "concat.hpp"
#include "json.hpp"
#include "to_string.hpp"
#include "fwd.hpp"

namespace kengine
{
    class GameObject : public putils::Mediator, public putils::Serializable<GameObject, false>
    {
    public:
        GameObject(std::string_view name) :
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
        friend class ComponentManager;

        template<typename CT>
        CT &attachComponent(std::unique_ptr<CT> &&comp)
        {
            if constexpr (!std::is_base_of<IComponent, CT>::value)
                static_assert("Attempt to attach something that's not a component");

            auto &ret = *comp;

            this->addModule(comp.get());
            const auto type = comp->getType();
            _components.emplace(type, std::move(comp));
            _types.push_back(type);

            return ret;
        }

        void detachComponent(const IComponent &comp)
        {
            const auto type = comp.getType();
            _components.erase(type);
            _types.erase(std::find(_types.begin(), _types.end(), type));
        }

        template<typename CT, typename ...Args>
        CT &attachComponent(Args &&...args)
        {
            if constexpr (!std::is_base_of<IComponent, CT>::value)
                static_assert("Attempt to attach something that's not a component");

            auto comp = std::make_unique<CT>(FWD(args)...);

            auto &ret = *comp;
            addModule(comp.get());
            const auto type = pmeta::type<CT>::index;
            _components.emplace(type, std::move(comp));
            _types.push_back(type);

            return ret;
        };

    public:
        template<class CT>
        CT &getComponent()
        {
            if constexpr (!std::is_base_of<IComponent, CT>::value)
                static_assert("Attempt to get something that's not a component");

            return static_cast<CT &>(*_components.at(pmeta::type<CT>::index));
        };

        template<class CT>
        const CT &getComponent() const
        {
            if constexpr (!std::is_base_of<IComponent, CT>::value)
                static_assert("Attempt to get something that's not a component");

            return static_cast<const CT &>(*_components.at(pmeta::type<CT>::index));
        };

    public:
        template<typename CT>
        bool hasComponent() const noexcept
        {
            if constexpr (!std::is_base_of<IComponent, CT>::value)
                static_assert("Attempt to get something that's not a component");

            return _components.find(pmeta::type<CT>::index) != _components.end();
        }

    public:
        std::string_view getName() const { return _name; }
        const std::vector<pmeta::type_index> &getTypes() const { return _types; }

    private:
        std::string _name;
        std::unordered_map<pmeta::type_index, std::unique_ptr<IComponent>> _components;
        std::vector<pmeta::type_index > _types;
    };
}
