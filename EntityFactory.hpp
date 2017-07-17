#pragma once

#include <string_view>
#include <memory>
#include <GameObject.hpp>

namespace kengine
{
    class EntityFactory
    {
    public:
        virtual ~EntityFactory() = default;

    public:
        virtual std::unique_ptr<GameObject> make(std::string_view type, std::string_view name) = 0;
    };

    class ExtensibleFactory : public EntityFactory
    {
    public:
        using Creator = std::function<std::unique_ptr<GameObject>(std::string_view name)>;
        void addType(std::string_view type, const Creator &creator)
        {
            _creators[type.data()] = creator;
        }

        template<typename T>
        void registerType()
        {
            static_assert(putils::is_reflectible<T>::value,
                          "Please make your type reflectible before registering it");
            _creators[T::get_class_name()] = [](auto name) { return std::make_unique<T>(name); };
        }

        template<typename ...Types>
        void registerTypes()
        {
            pmeta::tuple_for_each(std::make_tuple(pmeta::type<Types>()...),
                                  [this](auto &&t)
                                  {
                                      registerType<pmeta_wrapped(t)>();
                                  }
            );
        }

        std::unique_ptr<GameObject> make(std::string_view type, std::string_view name) final
        {
            return (_creators.at(type.data()))(name);
        }

    private:
        std::unordered_map<std::string, Creator> _creators;
    };
}