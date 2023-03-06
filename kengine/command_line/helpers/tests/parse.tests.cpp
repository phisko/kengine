// stl
#include <optional>

// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/command_line/data/arguments.hpp"
#include "kengine/command_line/helpers/parse.hpp"

namespace {
	struct options {
		std::optional<std::string> s;
	};
}

#define refltype options
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute(s)
	)
};
#undef refltype

TEST(command_line, parse) {
	entt::registry r;

	const auto e = r.create();
	r.emplace<kengine::command_line::arguments>(
		e,
		kengine::command_line::arguments{
			.args = { "--s=hello" },
		}
	);

	const auto args = kengine::command_line::parse<options>(r);
	EXPECT_NE(args.s, std::nullopt);
	EXPECT_EQ(*args.s, "hello");
}
