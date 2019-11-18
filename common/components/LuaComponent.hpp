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
#include "reflection.hpp"

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
        putils_reflection_class_name(LuaComponent);
        putils_reflection_attributes(
                putils_reflection_attribute(&LuaComponent::scripts)
        );
        putils_reflection_methods(
                putils_reflection_attribute(&LuaComponent::attachScript),
                putils_reflection_attribute(&LuaComponent::removeScript)
        );
    };

	struct LuaTableComponent {
		sol::table table;

		putils_reflection_class_name(LuaTableComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&LuaTableComponent::table)
		);
	};
}