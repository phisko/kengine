#pragma once

// fmt
#include <fmt/core.h>

// glm
#include <glm/glm.hpp>

template<>
struct fmt::formatter<glm::vec3> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const glm::vec3 & p, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({}, {}, {})", p.x, p.y, p.z);
	}
};

template<>
struct fmt::formatter<glm::vec4> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const glm::vec4 & p, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({}, {}, {}, {})", p.x, p.y, p.z, p.w);
	}
};
