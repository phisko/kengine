#include "tests/KengineTest.hpp"

// kengine helpers
#include "helpers/commandLineHelper.hpp"

struct commandLineHelper : KengineTest {};

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

TEST_F(commandLineHelper, createCommandLineEntity) {
	const char * args[] = {
		"hello",
		"hi"
	};
	kengine::createCommandLineEntity(2, args);

	for (const auto & [e, commandLine] : kengine::entities.with<kengine::CommandLineComponent>()) {
		EXPECT_EQ(commandLine.arguments.size(), 2);
		EXPECT_EQ(commandLine.arguments[0], "hello");
		EXPECT_EQ(commandLine.arguments[1], "hi");
	}
}

TEST_F(commandLineHelper, parseCommandLine) {
    kengine::entities += [](kengine::Entity & e) {
        e += kengine::CommandLineComponent{
            .arguments = { "--s=hello" }
        };
    };

    const auto options = kengine::parseCommandLine<Options>();
    EXPECT_NE(options.s, std::nullopt);
    EXPECT_EQ(*options.s, "hello");
}