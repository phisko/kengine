// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine helpers
#include "kengine/helpers/command_line_helper.hpp"

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

TEST(command_line_helper, create_command_line_entity) {
	entt::registry r;

	const char * args[] = {
		"hello",
		"hi"
	};
	kengine::create_command_line_entity(r, 2, args);

	for (const auto & [e, command_line] : r.view<kengine::data::command_line>().each()) {
		EXPECT_EQ(command_line.arguments.size(), 2);
		EXPECT_EQ(command_line.arguments[0], "hello");
		EXPECT_EQ(command_line.arguments[1], "hi");
	}
}

TEST(command_line_helper, parse_command_line) {
	entt::registry r;

	const auto e = r.create();
	r.emplace<kengine::data::command_line>(
		e,
		kengine::data::command_line{
			.arguments = { "--s=hello" },
		}
	);

	const auto args = kengine::parse_command_line<options>(r);
	EXPECT_NE(args.s, std::nullopt);
	EXPECT_EQ(*args.s, "hello");
}