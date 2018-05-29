#pragma once

#include <unordered_set>
#include <string_view>
#include <string>
#include <unordered_map>
#include <memory>
#include <type_traits>
#include "SystemManager.hpp"
#include "ComponentManager.hpp"
#include "EntityFactory.hpp"

namespace kengine {
    class EntityManager : public SystemManager, public ComponentManager, public putils::Serializable<EntityManager> {
    public:
        EntityManager(std::unique_ptr<EntityFactory> && factory = std::make_unique<ExtensibleFactory>())
                : _factory(std::move(factory)) {}

        ~EntityManager() = default;

    public:
        GameObject & createEntity(const std::string & type, const std::string & name,
                                  const std::function<void(GameObject &)> & postCreate = nullptr) {
            auto e = _factory->make(type, name);

            if (postCreate != nullptr)
                postCreate(*e);

            return addEntity(name, std::move(e));
        }

        GameObject & createEntity(const std::string & type, const std::function<void(GameObject &)> & postCreate = nullptr) {
            const auto it = _ids.find(type);
            if (it == _ids.end()) {
                _ids.emplace(type, 0);
                return createEntity(type, putils::concat(type, 0), postCreate);
            }
            return createEntity(type, putils::concat(type, ++it->second), postCreate);
        }

        template<class GO, typename ...Params>
        GO & createEntity(const std::string & name,
                          const std::function<void(GameObject &)> & postCreate = nullptr,
                          Params && ...params) {
            static_assert(std::is_base_of<GameObject, GO>::value,
                          "Attempt to create something that's not a GameObject");

            auto entity = std::make_unique<GO>(name, FWD(params)...);

            if (postCreate != nullptr)
                postCreate(static_cast<GameObject &>(*entity));

            return static_cast<GO &>(addEntity(name, std::move(entity)));
        }

        template<typename GO, typename ...Params>
        GO & createEntity(const std::function<void(GameObject &)> & postCreate = nullptr, Params && ...params) {
            static_assert(putils::is_reflectible<GO>::value, "createEntity must be given an explicit name if the type parameter is not reflectible.");

            const auto type = GO::get_class_name();
            const auto it = _ids.find(type);
            if (it == _ids.end()) {
                _ids.emplace(type, 0);
                return createEntity<GO>(putils::concat(type, 0), postCreate, FWD(params)...);
            }
            return createEntity<GO>(putils::concat(type, ++it->second), postCreate, FWD(params)...);
        }

    public:
		template<typename CompType, typename ...Params>
		kengine::GameObject & createOrAttach(const std::string & name, Params && ...args) {
			if (!hasEntity(name))
				return createEntity<kengine::GameObject>(name, [&name, &args...](kengine::GameObject & go) {
					go.attachComponent<CompType>(FWD(args)...);
				});
			else {
				auto & go = getEntity(name);
				go.attachComponent<CompType>(FWD(args)...);
				return go;
			}
		}

    private:
        GameObject & addEntity(const std::string & name, std::unique_ptr<GameObject> && obj) {
            auto & ret = *obj;
			_toAdd.push_back(std::move(obj));
			_futureEntities[name] = &ret;
            return ret;
        }

    public:
        void removeEntity(kengine::GameObject & go) noexcept {
            _toRemove.insert(&go);
        }

        void removeEntity(const std::string & name) noexcept {
            const auto p = _entities.find(name);
            if (p == _entities.end())
                return;
            _toRemove.insert(p->second.get());
        }

	public:
		const GameObject & getEntity(const std::string & name) const {
			const auto it = _futureEntities.find(name);
			if (it != _futureEntities.end())
				return *it->second;
			return *_entities.at(name);
		}

		GameObject & getEntity(const std::string & name) {
			const auto it = _futureEntities.find(name);
			if (it != _futureEntities.end())
				return *it->second;
			return *_entities.at(name);
		}

        bool hasEntity(const std::string & name) const noexcept { return _entities.find(name) != _entities.end(); }

    public:
        void addLink(const GameObject & parent, const GameObject & child) { _entityHierarchy[&child] = &parent; }

        void removeLink(const GameObject & child) { _entityHierarchy.erase(&child); }

        const GameObject & getParent(const GameObject & go) const { return *_entityHierarchy.at(&go); }

    public:
        template<typename T>
        T & getFactory() { return static_cast<T &>(*_factory); }

        template<typename T>
        const T & getFactory() const { return static_cast<const T &>(*_factory); }

    public:
        template<typename RegisterWith, typename ...Types>
        void registerTypes() {
            if constexpr (!std::is_same<RegisterWith, nullptr_t>::value) {
                try {
                    auto & s = getSystem<RegisterWith>();
                    s.template registerTypes<Types...>();
                }
                catch (const std::out_of_range &) {}
            }

			try {
				auto & factory = getFactory<kengine::ExtensibleFactory>();
				pmeta_for_each(Types, [this pmeta_comma &factory](auto && t) {
					using Type = pmeta_wrapped(t);
					if constexpr (std::is_base_of<kengine::GameObject, Type>::value)
						factory.registerType<Type>();
					else if constexpr (kengine::is_component<Type>::value)
						registerCompLoader<Type>();
				});
			}
			catch (const std::out_of_range &) {}
		}

    public:
        void execute(const std::function<void()> & betweenSystems = []{}) noexcept {
			updateEntities();
            SystemManager::execute([this, &betweenSystems] {
				updateEntities();
                betweenSystems();
            });
        }

    public:
		bool isEntityEnabled(GameObject & go) noexcept { return _disabled.find(&go) == _disabled.end(); }
		bool isEntityEnabled(const std::string & name) noexcept { return isEntityEnabled(getEntity(name)); }

		void disableEntity(GameObject & go) noexcept {
			_toDisable.emplace(&go);
		}

		void disableEntity(const std::string & name) { disableEntity(getEntity(name)); }

		void enableEntity(GameObject & go) noexcept {
			ComponentManager::registerGameObject(go);
			SystemManager::registerGameObject(go);
			_disabled.erase(&go);
		}

		void enableEntity(const std::string & name) { enableEntity(getEntity(name)); }

    public:
		using CompLoader = std::function<void(kengine::GameObject &, const putils::json &)>;

		template<typename T>
		void registerCompLoader(const CompLoader & loader) {
			_loaders[T::get_class_name()] = loader;
		}

		template<typename T>
		void registerCompLoader() {
			if constexpr (kengine::is_component<T>::value)
				_loaders[T::get_class_name()] = [](kengine::GameObject & go, const putils::json & json) {
					auto & comp = go.attachComponent<T>();
					putils::parse(comp, json.dump());
				};
		}

		void onLoad(const std::function<void()> & func) {
			_onLoad.push_back(func);
		}

		void save(const std::string & file) {
			std::ofstream f(file, std::ofstream::trunc);

			if (!f)
				return;
			f << "{";
			putils::OutputPolicies::Json::serialize(f, "ids", _ids);
			f << ", \"entities\": [";
			bool first = true;
			for (const auto & go : getGameObjects()) {
				if (!first)
					f << ",";
				f << *go;
				first = false;
			}
			f << "]}";
		}

		void load(const std::string & file) {
			std::ifstream f(file);

			if (!f)
				return;

			const auto jsonObj = putils::json::parse(f);

			std::stringstream s(jsonObj["ids"].dump());
			putils::OutputPolicies::Json::unserialize(s, _ids);

			for (const auto &[name, go] : _entities)
				removeEntity(*go);

			for (const auto & entity : jsonObj["entities"])
				loadEntity(entity);

			_justLoaded = true;
		}

    public:
		void loadEntity(const putils::json & jsonObject) {
			const std::string & name = jsonObject["name"];
			if (hasEntity(name))
				removeEntity(name);
			createEntity<kengine::GameObject>(name, [this, &jsonObject](kengine::GameObject & go) { loadComponents(jsonObject, go); });
		}

    public:
		void loadEntity(const std::string & jsonString) { loadEntity(putils::json::parse(jsonString)); }

    private:
		void loadComponents(const putils::json & obj, kengine::GameObject & go) {
			for (auto it = obj["components"].begin(); it != obj["components"].end(); ++it) {
				const auto & element = it.value();
				const auto type = element.find("type");
				if (type == element.end())
					continue;

				const auto loader = _loaders.find(type.value());
				if (loader != _loaders.end())
					loader->second(go, element);
			}
		}

    private:
		void updateEntities() noexcept {
			doRemove();
			updateEntitiesByType();
			doAdd();
			doDisable();
			updateEntitiesByType();

			if (_justLoaded) {
				for (const auto & func : _onLoad) {
					try {
						func();
					}
					catch (const std::exception & e) { std::cerr << e.what() << std::endl; }
				}

				for (const auto go : getGameObjects())
					if (go->_components.empty())
						removeEntity(*go);

				_justLoaded = false;
			}

			updateEntitiesByType();
		}

	private:
		void doAdd() noexcept {
			for (auto && go : _toAdd) {
				const auto name = go->getName();
				const auto it = _entities.find(name);
				if (it != _entities.end()) {
					ComponentManager::removeGameObject(*it->second);
					SystemManager::removeGameObject(*it->second);
				}
				auto & obj = *go;
				_entities[name] = std::move(go);

				SystemManager::registerGameObject(obj);
				ComponentManager::registerGameObject(obj);
			}
			_toAdd.clear();
			_futureEntities.clear();
		}

        void doRemove() noexcept {
            while (!_toRemove.empty()) {
                const auto tmp = _toRemove;
                _toRemove.clear();

                for (const auto go : tmp) {
                    SystemManager::removeGameObject(*go);
                    ComponentManager::removeGameObject(*go);
					const auto it = _entities.find(go->getName());
					if (it != _entities.end())
						_entities.erase(it);
                }

                for (const auto go : tmp)
                    _toRemove.erase(go);
            }
        }

    private:
		void doDisable() noexcept {
			while (!_toDisable.empty()) {
				const auto tmp = _toDisable;
				_toDisable.clear();

				for (const auto go : tmp) {
					SystemManager::removeGameObject(*go);
					ComponentManager::removeGameObject(*go);
					_disabled.emplace(go);
				}

				for (const auto go : tmp)
					_toDisable.erase(go);
			}
		}

	private:
		std::unordered_map<std::string, CompLoader> _loaders;
		std::vector<std::function<void()>> _onLoad;
		bool _justLoaded = false;

    private:
        std::unique_ptr<EntityFactory> _factory;
        std::unordered_map<std::string, std::size_t> _ids;

    private:
        std::unordered_map<std::string, std::unique_ptr<GameObject>> _entities;

        std::vector<std::unique_ptr<GameObject>> _toAdd;
        std::unordered_map<std::string, GameObject *> _futureEntities;

        std::unordered_set<GameObject *> _toRemove;

        std::unordered_map<const GameObject *, const GameObject *> _entityHierarchy;

    private:
        std::unordered_set<GameObject *> _toDisable;
        std::unordered_set<GameObject *> _disabled;

    public:
		pmeta_get_class_name(EntityManager);
		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&EntityManager::_ids),
			pmeta_reflectible_attribute_private(&EntityManager::_entities)
		);
		pmeta_get_methods();
		pmeta_get_parents();
    };
}
