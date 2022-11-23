// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine helpers
#include "helpers/commandLineHelper.hpp"

namespace {
    struct Options {
        std::optional<std::string> s;
    };
}

#define refltype Options
putils_reflection_info {
    putils_reflection_attributes(
        putils_reflection_attribute(s)
    )
};
#undef refltype

TEST(commandLineHelper, createCommandLineEntity) {
	entt::registry r;

	const char * args[] = {
		"hello",
		"hi"
	};
	kengine::createCommandLineEntity(r, 2, args);

	for (const auto & [e, commandLine] : r.view<kengine::CommandLineComponent>().each()) {
		EXPECT_EQ(commandLine.arguments.size(), 2);
		EXPECT_EQ(commandLine.arguments[0], "hello");
		EXPECT_EQ(commandLine.arguments[1], "hi");
	}
}

TEST(commandLineHelper, parseCommandLine) {
	entt::registry r;

	const auto e = r.create();
	r.emplace<kengine::CommandLineComponent>(e, kengine::CommandLineComponent{
		.arguments = {"--s=hello"}
	});

    const auto options = kengine::parseCommandLine<Options>(r);
    EXPECT_NE(options.s, std::nullopt);
    EXPECT_EQ(*options.s, "hello");
}