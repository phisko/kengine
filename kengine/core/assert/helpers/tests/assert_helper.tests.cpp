// gtest
#include <gtest/gtest.h>

// entt
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/assert/functions/on_assert_failed.hpp"
#include "kengine/core/assert/helpers/kengine_assert.hpp"

struct core_assert : ::testing::Test {
	core_assert() noexcept {
		r.emplace<kengine::core::assert::on_assert_failed>(
			r.create(),
			[this](const char * file, int line, const std::string & expr) {
				this->file = file;
				this->line = line;
				this->expr = expr;
				return false; // Don't break the debugger
			}
		);
	}

	entt::registry r;
	const char * file = nullptr;
	int line = -1;
	std::string expr;
};

TEST_F(core_assert, kengine_assert_true) {
	kengine_assert(r, true);
	EXPECT_EQ(file, nullptr);
	EXPECT_EQ(line, -1);
	EXPECT_EQ(expr, "");
}

TEST_F(core_assert, kengine_assert_false) {
	kengine_assert(r, false);
	EXPECT_STREQ(file, __FILE__);
	EXPECT_EQ(line, 38); // Hardcoded line number based on the above condition
	EXPECT_EQ(expr, "false");
}

TEST_F(core_assert, kengine_assert_false_condition) {
	int i = 0;
	int j = 42;
	kengine_assert(r, i == j);
	EXPECT_STREQ(file, __FILE__);
	EXPECT_EQ(line, 47); // Hardcoded line number based on the above condition
	EXPECT_EQ(expr, "i == j");
}

TEST_F(core_assert, kengine_assert_with_message_true) {
	kengine_assert_with_message(r, true, "foo");
	EXPECT_EQ(file, nullptr);
	EXPECT_EQ(line, -1);
	EXPECT_EQ(expr, "");
}

TEST_F(core_assert, kengine_assert_with_message_false) {
	kengine_assert_with_message(r, false, "{}{}", "foo", "bar");
	EXPECT_STREQ(file, __FILE__);
	EXPECT_EQ(line, 61); // Hardcoded line number based on the above condition
	EXPECT_EQ(expr, "foobar");
}

TEST_F(core_assert, kengine_assert_failed) {
	kengine_assert_failed(r, "{}{}", "foo", "bar");
	EXPECT_STREQ(file, __FILE__);
	EXPECT_EQ(line, 68); // Hardcoded line number based on the above condition
	EXPECT_EQ(expr, "foobar");
}
