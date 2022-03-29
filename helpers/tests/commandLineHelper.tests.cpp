#include <gtest/gtest.h>
#include "tests/KengineTest.hpp"

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