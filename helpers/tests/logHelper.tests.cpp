// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine helpers
#include "helpers/logHelper.hpp"

TEST(logHelper, logHelper) {
	entt::registry r;

    struct Event {
        kengine::LogSeverity severity;
        std::string category;
        std::string message;
    };
    std::vector<Event> output;

	const auto e = r.create();
	r.emplace<kengine::functions::Log>(
		e, [&](const kengine::LogEvent & log) {
			output.push_back({
                .severity = log.severity,
                .category = log.category,
                .message = log.message
            });
		}
	);

    kengine_log(r, Verbose, "Category", "Message");
    kengine_logf(r, Warning, "OtherCategory", "%s", "OtherMessage");

    EXPECT_EQ(output.size(), 2);

    EXPECT_EQ(output[0].severity, kengine::LogSeverity::Verbose);
    EXPECT_EQ(output[0].category, "Category");
    EXPECT_EQ(output[0].message, "Message");

    EXPECT_EQ(output[1].severity, kengine::LogSeverity::Warning);
    EXPECT_EQ(output[1].category, "OtherCategory");
    EXPECT_EQ(output[1].message, "OtherMessage");
}
