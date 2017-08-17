#pragma once

#include <string>
#include "lua/sol.hpp"

#include "SerializableComponent.hpp"

namespace kengine
{
    class LuaComponent : public putils::Reflectible<LuaComponent>,
                         public SerializableComponent<LuaComponent>
    {
    public:
        LuaComponent(const std::vector<std::string> &scripts = {})
                : _scripts(scripts)
        {
        }

    public:
        sol::object meta;
        std::string debug;

    public:
        void attachScript(std::string_view file) noexcept { _scripts.push_back(file.data()); }

        void removeScript(std::string_view file) noexcept
        {
            _scripts.erase(std::find(_scripts.begin(), _scripts.end(), file.data()));
        }

    public:
        const std::vector<std::string> &getScripts() const noexcept { return _scripts; }

    private:
        const std::string type = pmeta_nameof(LuaComponent);
        std::vector<std::string> _scripts;

        /*
         * Reflectible
         */
    public:
        static const auto get_class_name() { return pmeta_nameof(LuaComponent); }

        static const auto &get_attributes()
        {
            static const auto table = pmeta::make_table(
                    pmeta_reflectible_attribute(&LuaComponent::type),
                    pmeta_reflectible_attribute_private(&LuaComponent::_scripts),
                    pmeta_reflectible_attribute(&LuaComponent::meta),
                    pmeta_reflectible_attribute(&LuaComponent::debug)
            );
            return table;
        }

        static const auto &get_methods()
        {
            static const auto table = pmeta::make_table(
                    pmeta_reflectible_attribute(&LuaComponent::attachScript),
                    pmeta_reflectible_attribute(&LuaComponent::removeScript)
            );
            return table;
        }

        static const auto &get_parents()
        {
            static const auto table = pmeta::make_table(
            );
            return table;
        }
    };
}