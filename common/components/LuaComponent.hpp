#pragma once

#ifndef KENGINE_MAX_LUA_SCRIPT_PATH
# define KENGINE_MAX_LUA_SCRIPT_PATH 64
#endif

#ifndef KENGINE_MAX_LUA_SCRIPTS
# define KENGINE_MAX_LUA_SCRIPTS 8
#endif

#include "string.hpp"
#include "vector.hpp"
#include "lua/sol.hpp"
#include "reflection/Reflectible.hpp"
#include "not_serializable.hpp"

namespace kengine {
    struct LuaComponent {
	public:
		static constexpr char stringName[] = "LuaComponentString";
		using script = putils::string<KENGINE_MAX_LUA_SCRIPT_PATH, stringName>;
		static constexpr char vectorName[] = "LuaComponentVector";
		using script_vector = putils::vector<script, KENGINE_MAX_LUA_SCRIPTS>;

        void attachScript(const char * file) noexcept { scripts.push_back(file); }

        void removeScript(const char * file) noexcept {
            scripts.erase(std::find(scripts.begin(), scripts.end(), file));
        }

        const auto & getScripts() const noexcept { return scripts; }

        script_vector scripts;

        /*
         * Reflectible
         */
    public:
        pmeta_get_class_name(LuaComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&LuaComponent::scripts)
        );
        pmeta_get_methods(
                pmeta_reflectible_attribute(&LuaComponent::attachScript),
                pmeta_reflectible_attribute(&LuaComponent::removeScript)
        );
        pmeta_get_parents();
    };

	struct LuaTableComponent : kengine::not_serializable {
		sol::table table;

		pmeta_get_class_name(LuaTableComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&LuaTableComponent::table)
		);
	};
}