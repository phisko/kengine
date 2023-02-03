#pragma once

// entt
#include <entt/entity/handle.hpp>

// kengine functions
#include "kengine/functions/create_system.hpp"

// kengine helpers
#include "kengine/helpers/meta/register_storage.hpp"

#define DECLARE_KENGINE_SYSTEM_CREATOR(EXPORT_MACRO, system_name) \
	EXPORT_MACRO entt::entity add_##system_name(entt::registry & r) noexcept;\
	EXPORT_MACRO entt::entity register_##system_name(entt::registry & r) noexcept;

#define DEFINE_KENGINE_SYSTEM_CREATOR(system_name, ...) \
	entt::entity add_##system_name(entt::registry & r) noexcept {\
		const auto e = register_##system_name(r);\
		const entt::handle handle{ r, e };\
		(void)handle.get_or_emplace<system_name>(handle);\
		return e;\
	}\
\
	entt::entity register_##system_name(entt::registry & r) noexcept {\
		register_storage<system_name>(r);\
		register_storage<__VA_ARGS__>(r);\
\
		const entt::handle e{ r, r.create() };\
		e.emplace<functions::create_system>([e](entt::registry & r) {\
			if (e.registry() == &r) {\
				(void)e.get_or_emplace<system_name>(e);\
				return e.entity();\
			}\
			return add_##system_name(r);\
		});\
		return e;\
	}