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
}