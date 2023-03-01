// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"

TEST(log, kengine_log) {
	entt::registry r;

	struct event {
		kengine::core::log::severity message_severity;
		std::string category;
		std::string message;
	};
	std::vector<event> output;

	const auto e = r.create();
	r.emplace<kengine::core::log::on_log>(
		e, [&](const kengine::core::log::event & event) {
			output.push_back({
				.message_severity = event.message_severity,
				.category = event.category,
				.message = event.message,
			});
		}
	);

	kengine_log(r, verbose, "Category", "Message");
	kengine_logf(r, warning, "OtherCategory", "%s", "OtherMessage");

	EXPECT_EQ(output.size(), 2);

	EXPECT_EQ(output[0].message_severity, kengine::core::log::severity::verbose);
	EXPECT_EQ(output[0].category, "Category");
	EXPECT_EQ(output[0].message, "Message");

	EXPECT_EQ(output[1].message_severity, kengine::core::log::severity::warning);
	EXPECT_EQ(output[1].category, "OtherCategory");
	EXPECT_EQ(output[1].message, "OtherMessage");
}
