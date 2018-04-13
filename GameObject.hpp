#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <algorithm>
#include <memory>

#include "IComponent.hpp"
#include "Mediator.hpp"

#include "reflection/Serializable.hpp"

#include "concat.hpp"
#include "json.hpp"
#include "to_string.hpp"
#include "fwd.hpp"

namespace kengine {
    class ComponentManager;
	class EntityManager;

    class GameObject : public putils::Mediator,
                       public putils::Reflectible<GameObject>,
                       public putils::Serializable<GameObject> {
    public:
        GameObject(std::string_view name) : _name(name) {}

        GameObject(GameObject && other) = default;
        GameObject & operator=(GameObject && other) = default;
        ~GameObject() = default;

    public:
        template<typename CT>
        CT & attachComponent(std::unique_ptr<CT> && comp);

        template<typename CT>
        void detachComponent();

        template<typename CT>
        void detachComponent(const CT & comp);

        template<typename CT, typename ...Args>
        CT & attachComponent(Args && ...args) {
            static_assert(std::is_base_of<IComponent, CT>::value,
                          "Attempt to attach something that's not a component");
            return attachComponent(std::make_unique<CT>(FWD(args)...));
        }

    public:
        template<class CT>
        CT & getComponent() {
            static_assert(std::is_base_of<IComponent, CT>::value,
                          "Attempt to get something that's not a component");
            return static_cast<CT &>(*_components.at(pmeta::type<CT>::index));
        };

        template<class CT>
        const CT & getComponent() const {
            static_assert(std::is_base_of<IComponent, CT>::value,
                          "Attempt to get something that's not a component");
            return static_cast<const CT &>(*_components.at(pmeta::type<CT>::index));
        };

    public:
        template<typename CT>
        bool hasComponent() const noexcept {
            static_assert(std::is_base_of<IComponent, CT>::value,
                          "Attempt to get something that's not a component");
            return _components.find(pmeta::type<CT>::index) != _components.end();
        }

    public:
        const std::string & getName() const { return _name; }
        const std::vector<pmeta::type_index> & getTypes() const { return _types; }

    private:
        friend class ComponentManager;

        ComponentManager * _manager = nullptr;
        void setManager(ComponentManager * manager) {
            _manager = manager;
        }

    private:
        std::string _name;
        std::unordered_map<pmeta::type_index, std::shared_ptr<IComponent>> _components;
        std::vector<pmeta::type_index> _types;

        /*
         * Reflectible
         */

    public:
        pmeta_get_class_name(GameObject);

        pmeta_get_attributes(
                pmeta_reflectible_attribute_private(&GameObject::_name),
                pmeta_reflectible_attribute_private(&GameObject::_components)
        );

        pmeta_get_methods(
                pmeta_reflectible_attribute(&GameObject::getName)
        );

        pmeta_get_parents(
                pmeta_reflectible_parent(putils::Mediator)
        );
    };
}

#include "ComponentManager.hpp"

template<typename CT>
void kengine::GameObject::detachComponent() {
    static_assert(std::is_base_of<IComponent, CT>::value,
                  "Attempt to detach something that's not a component");

    const auto type = pmeta::type<CT>::index;

    if (_manager)
        _manager->removeComponent(*this, type);

    _components.erase(type);
    _types.erase(std::find(_types.begin(), _types.end(), type));
}

template<typename CT>
void kengine::GameObject::detachComponent(const CT & comp) {
    static_assert(std::is_base_of<IComponent, CT>::value,
                  "Attempt to detach something that's not a component");

    const auto type = comp.getType();

    if (_manager)
        _manager->removeComponent(*this, type);

    _components.erase(type);
    _types.erase(std::find(_types.begin(), _types.end(), type));
}

template<typename CT>
CT & kengine::GameObject::attachComponent(std::unique_ptr<CT> && comp) {
    static_assert(std::is_base_of<IComponent, CT>::value,
                  "Attempt to attach something that's not a component");

    auto & ret = *comp;

    addModule(*comp);
    const auto type = comp->getType();
	const auto it = _components.find(type);
	if (it != _components.end()) {
		_types.erase(std::find(_types.begin(), _types.end(), type));
		if (_manager)
			_manager->removeComponent(*this, type);
	}
    _components[type] = std::move(comp);
    _types.push_back(type);

    if (_manager)
        _manager->registerComponent(*this, ret);

    return ret;
}

