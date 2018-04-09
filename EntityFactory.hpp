#pragma once

#include <string_view>
#include <memory>
#include <GameObject.hpp>


namespace kengine {
    class EntityFactory {
    public:
        virtual ~EntityFactory() = default;

    public:
        virtual std::unique_ptr<GameObject> make(const std::string & type, const std::string & name) = 0;
    };

    class ExtensibleFactory : public EntityFactory {
    public:
        ExtensibleFactory() {
            registerType<kengine::GameObject>();
        }

        using Creator = std::function<std::unique_ptr<GameObject>(const std::string & name)>;
        void addType(const std::string & type, const Creator & creator) {
            _creators[type] = creator;
        }

        template<typename T>
        void registerType() {
            static_assert(putils::is_reflectible<T>::value,
                          "Please make your type reflectible before registering it");
            _creators[T::get_class_name()] = [](const auto & name) { return std::make_unique<T>(name); };
        }

        template<typename ...Types>
        void registerTypes() {
            pmeta::tuple_for_each(
                    std::make_tuple(pmeta::type<Types>()...),
                    [this](auto && t) { registerType<pmeta_wrapped(t)>(); }
            );
        }

        std::unique_ptr<GameObject> make(const std::string & type, const std::string & name) final {
            return (_creators.at(type))(name);
        }

    private:
        std::unordered_map<std::string, Creator> _creators;
    };
}