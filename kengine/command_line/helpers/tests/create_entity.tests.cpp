// stl
#include <optional>

// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/command_line/data/arguments.hpp"
#include "kengine/command_line/helpers/create_entity.hpp"

TEST(command_line, create_entity) {
	entt::registry r;

	const char * args[] = {
		"hello",
		"hi"
	};
	kengine::command_line::create_entity(r, 2, args);

	for (const auto & [e, command_line] : r.view<kengine::command_line::arguments>().each()) {
		EXPECT_EQ(command_line.args.size(), 2);
		EXPECT_EQ(command_line.args[0], "hello");
		EXPECT_EQ(command_line.args[1], "hi");
	}
}