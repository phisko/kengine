#pragma once

// entt
#include <entt/entity/handle.hpp>

// kengine functions
#include "kengine/system_creator/functions/create_system.hpp"

// kengine helpers
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/meta/helpers/register_storage.hpp"

#define DECLARE_KENGINE_SYSTEM_CREATOR(EXPORT_MACRO, system_name) \
	EXPORT_MACRO entt::entity add_##system_name(entt::registry & r) noexcept; \
	EXPORT_MACRO entt::entity register_##system_name(entt::registry & r) noexcept;

#define DEFINE_KENGINE_SYSTEM_CREATOR(system_name, ...) \
	entt::entity add_##system_name(entt::registry & r) noexcept; \
	entt::entity register_##system_name(entt::registry & r) noexcept; \
	entt::entity add_##system_name(entt::registry & r) noexcept { \
		KENGINE_PROFILING_SCOPE; \
		const auto e = register_##system_name(r); \
		kengine_logf(r, verbose, "system_creator", "Constructing " #system_name " system in {}", e); \
\
		const entt::handle handle{ r, e }; \
		(void)handle.get_or_emplace<system_name>(handle); \
		return e; \
	} \
\
	entt::entity register_##system_name(entt::registry & r) noexcept { \
		KENGINE_PROFILING_SCOPE; \
\
		kengine::meta::register_storage<system_name>(r); \
		kengine::meta::register_storage<__VA_ARGS__>(r); \
\
		const entt::handle e{ r, r.create() }; \
		kengine_logf(r, verbose, "system_creator", "Registering " #system_name " system as {}", e); \
\
		e.emplace<kengine::system_creator::create_system>([e](entt::registry & r) { \
			if (e.registry() == &r) { \
				kengine_logf(r, verbose, "system_creator", "Constructing " #system_name " system in {}", e); \
				(void)e.get_or_emplace<system_name>(e); \
				return e.entity(); \
			} \
			return add_##system_name(r); \
		}); \
		return e; \
	}
