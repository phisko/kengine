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

        std::unique_ptr<GameObject> make(std::string_view type, std::string_view name) final
        {
            return (_creators.at(type.data()))(name);
        }

    private:
        std::unordered_map<std::string, Creator> _creators;
    };
}