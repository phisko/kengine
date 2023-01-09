#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<bool IsComponent, typename... Comps>
	void register_with_script_languages(entt::registry & r) noexcept;

	template<typename F>
	void register_with_script_languages(const entt::registry & r, const char * name, F && func) noexcept;
}

#include "register_with_script_languages.inl"