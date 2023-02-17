// gtest
#include <gtest/gtest.h>

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/helpers/assert_helper.hpp"

struct assert_helper : ::testing::Test {
	assert_helper() noexcept {
		kengine::assert_helper::assert_handler = [this](const entt::registry & r, const char * file, int line, const std::string & expr) {
			this->file = file;
			this->line = line;
			this->expr = expr;
			return false; // Don't break the debugger
		};
	}

	entt::registry r;
	const char * file = nullptr;
	int line = -1;
	std::string expr;
};

TEST_F(assert_helper, kengine_assert_true) {
	kengine_assert(r, true);
	EXPECT_EQ(file, nullptr);
	EXPECT_EQ(line, -1);
	EXPECT_EQ(expr, "");
}

TEST_F(assert_helper, kengine_assert_false) {
	kengine_assert(r, false);
	EXPECT_STREQ(file, __FILE__);
	EXPECT_EQ(line, 34); // Hardcoded line number based on the above condition
	EXPECT_EQ(expr, "false");
}

TEST_F(assert_helper, kengine_assert_false_condition) {
	int i = 0;
	int j = 42;
	kengine_assert(r, i == j);
	EXPECT_STREQ(file, __FILE__);
	EXPECT_EQ(line, 43); // Hardcoded line number based on the above condition
	EXPECT_EQ(expr, "i == j");
}

TEST_F(assert_helper, kengine_assert_with_message_true) {
	kengine_assert_with_message(r, true, "foo");
	EXPECT_EQ(file, nullptr);
	EXPECT_EQ(line, -1);
	EXPECT_EQ(expr, "");
}

TEST_F(assert_helper, kengine_assert_with_message_false) {
	kengine_assert_with_message(r, false, "foo", "bar");
	EXPECT_STREQ(file, __FILE__);
	EXPECT_EQ(line, 57); // Hardcoded line number based on the above condition
	EXPECT_EQ(expr, "foobar");
}

TEST_F(assert_helper, kengine_assert_failed) {
	kengine_assert_failed(r, "foo", "bar");
	EXPECT_STREQ(file, __FILE__);
	EXPECT_EQ(line, 64); // Hardcoded line number based on the above condition
	EXPECT_EQ(expr, "foobar");
}
