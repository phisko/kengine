// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"

TEST(log_helper, log_helper) {
	entt::registry r;

	struct event {
		kengine::log_severity severity;
		std::string category;
		std::string message;
	};
	std::vector<event> output;

	const auto e = r.create();
	r.emplace<kengine::functions::log>(
		e, [&](const kengine::log_event & log) {
			output.push_back({
				.severity = log.severity,
				.category = log.category,
				.message = log.message,
			});
		}
	);

	kengine_log(r, verbose, "Category", "Message");
	kengine_logf(r, warning, "OtherCategory", "%s", "OtherMessage");

	EXPECT_EQ(output.size(), 2);

	EXPECT_EQ(output[0].severity, kengine::log_severity::verbose);
	EXPECT_EQ(output[0].category, "Category");
	EXPECT_EQ(output[0].message, "Message");

	EXPECT_EQ(output[1].severity, kengine::log_severity::warning);
	EXPECT_EQ(output[1].category, "OtherCategory");
	EXPECT_EQ(output[1].message, "OtherMessage");
}
