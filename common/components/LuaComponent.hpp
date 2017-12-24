#pragma once

#include <string>
#include "lua/sol.hpp"

#include "SerializableComponent.hpp"

namespace kengine {
    class LuaComponent : public putils::Reflectible<LuaComponent>,
                         public SerializableComponent<LuaComponent> {
    public:
        LuaComponent(const std::vector<std::string> & scripts = {})
                : _scripts(scripts) {
        }

    public:
        sol::object meta;
        std::string debug;

    public:
        void attachScript(const std::string & file) noexcept { _scripts.push_back(file); }

        void removeScript(const std::string & file) noexcept {
            _scripts.erase(std::find(_scripts.begin(), _scripts.end(), file));
        }

    public:
        const std::vector<std::string> & getScripts() const noexcept { return _scripts; }

    private:
        const std::string type = pmeta_nameof(LuaComponent);
        std::vector<std::string> _scripts;

        /*
         * Reflectible
         */
    public:
        pmeta_get_class_name(LuaComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&LuaComponent::type),
                pmeta_reflectible_attribute_private(&LuaComponent::_scripts),
                pmeta_reflectible_attribute(&LuaComponent::meta),
                pmeta_reflectible_attribute(&LuaComponent::debug)
        );
        pmeta_get_methods(
                pmeta_reflectible_attribute(&LuaComponent::attachScript),
                pmeta_reflectible_attribute(&LuaComponent::removeScript)
        );
        pmeta_get_parents();
    };
}