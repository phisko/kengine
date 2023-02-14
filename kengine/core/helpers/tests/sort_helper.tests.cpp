// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine core
#include "kengine/core/helpers/sort_helper.hpp"

struct sort_helper : ::testing::Test {
	struct entity_data {
		std::string s;
		int i = 0;
		kengine::data::name name;
	};

	entt::registry r;

	const std::vector<entity_data> data{
		{ .s = "hello", .i = 1, .name = { "B" } },
		{ .s = "hi", .i = 0, .name = { "A" } }
	};

	const std::vector<entity_data> sorted_data = [this] {
		auto ret = data;
		std::ranges::sort(ret, [](const auto & lhs, const auto & rhs) { return lhs.i < rhs.i; });
		return ret;
	}();

	sort_helper() {
		for (const auto & d : data) {
			const auto e = r.create();
			r.emplace<std::string>(e, d.s);
			r.emplace<int>(e, d.i);
			r.emplace<kengine::data::name>(e, d.name);
		}
	}
};

TEST_F(sort_helper, get_sorted_entities_std_vector) {
	const auto vec = kengine::sort_helper::get_sorted_entities<const int, const std::string>(r, [](const auto & lhs, const auto & rhs) {
		return *std::get<1>(lhs) < *std::get<1>(rhs);
	});

	static_assert(putils::is_specialization<putils_typeof(vec), std::vector>());
	EXPECT_EQ(vec.size(), data.size());

	size_t count = 0;
	for (const auto & [e, i, s] : vec) {
		EXPECT_EQ(*s, sorted_data[count].s);
		EXPECT_EQ(*i, sorted_data[count].i);
		++count;
	}
}

TEST_F(sort_helper, get_sorted_entities_putils_vector) {
	const auto vec = kengine::sort_helper::get_sorted_entities<3, const int, const std::string>(r, [](const auto & lhs, const auto & rhs) {
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

TEST_F(sort_helper, get_name_sorted_entities_std_vector) {
	const auto vec = kengine::sort_helper::get_name_sorted_entities<const int, const std::string>(r);

	static_assert(putils::is_specialization<putils_typeof(vec), std::vector>());
	EXPECT_EQ(vec.size(), data.size());

	size_t count = 0;
	for (const auto & [e, name, i, s] : vec) {
		EXPECT_EQ(name->name, sorted_data[count].name.name);
		EXPECT_EQ(*s, sorted_data[count].s);
		EXPECT_EQ(*i, sorted_data[count].i);
		++count;
	}
}

TEST_F(sort_helper, get_name_sorted_entities_putils_vector) {
	const auto vec = kengine::sort_helper::get_name_sorted_entities<3, const int, const std::string>(r);

	static_assert(putils::is_vector<putils_typeof(vec)>());
	EXPECT_EQ(vec.size(), data.size());

	size_t count = 0;
	for (const auto & [e, name, i, s] : vec) {
		EXPECT_EQ(name->name, sorted_data[count].name.name);
		EXPECT_EQ(*s, sorted_data[count].s);
		EXPECT_EQ(*i, sorted_data[count].i);
		++count;
	}
}
