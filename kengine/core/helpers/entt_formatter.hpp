#pragma once

// fmt
#include <fmt/core.h>

// entt
#include <entt/entity/entity.hpp>
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

template<>
struct fmt::formatter<entt::entity> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(entt::entity e, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "[{}]", std::underlying_type_t<entt::entity>(e));
	}
};

template<>
struct fmt::formatter<entt::handle> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(entt::handle e, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "{}", e.entity());
	}
};

template<>
struct fmt::formatter<entt::const_handle> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(entt::const_handle e, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "{}", e.entity());
	}
};
