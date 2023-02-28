// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine core
#include "kengine/core/sort/helpers/get_sorted_entities.hpp"

struct core_sort : ::testing::Test {
	struct entity_data {
		std::string s;
		int i = 0;
	};

	entt::registry r;

	const std::vector<entity_data> data{
		{ .s = "hello", .i = 1 },
		{ .s = "hi", .i = 0 }
	};

	const std::vector<entity_data> sorted_data = [this] {
		auto ret = data;
		std::ranges::sort(ret, [](const auto & lhs, const auto & rhs) { return lhs.i < rhs.i; });
		return ret;
	}();

	core_sort() {
		for (const auto & d : data) {
			const auto e = r.create();
			r.emplace<std::string>(e, d.s);
			r.emplace<int>(e, d.i);
		}
	}
};

TEST_F(core_sort, get_sorted_entities_std_vector) {
	const auto vec = kengine::core::sort::get_sorted_entities<const int, const std::string>(r, [](const auto & lhs, const auto & rhs) {
		return *std::get<1>(lhs) < *std::get<1>(rhs);
	});

	static_assert(putils::specialization<putils_typeof(vec), std::vector>);
	EXPECT_EQ(vec.size(), data.size());

	size_t count = 0;
	for (const auto & [e, i, s] : vec) {
		EXPECT_EQ(*s, sorted_data[count].s);
		EXPECT_EQ(*i, sorted_data[count].i);
		++count;
	}
}

TEST_F(core_sort, get_sorted_entities_putils_vector) {
	const auto vec = kengine::core::sort::get_sorted_entities<3, const int, const std::string>(r, [](const auto & lhs, const auto & rhs) {
		return *std::get<1>(lhs) < *std::get<1>(rhs);
	});

	static_assert(putils::is_vector<putils_typeof(vec)>());
	EXPECT_EQ(vec.size(), data.size());

	size_t count = 0;
	for (const auto & [e, i, s] : vec) {
		EXPECT_EQ(*s, sorted_data[count].s);
		EXPECT_EQ(*i, sorted_data[count].i);
		++count;
	}
}