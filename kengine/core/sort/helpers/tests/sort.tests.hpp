// gtest
#include <gtest/gtest.h>

// entt
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/data/name.hpp"

struct core_sort : ::testing::Test {
	struct entity_data {
		std::string s;
		int i = 0;
		kengine::core::name name;
	};

	entt::registry r;

	const std::vector<entity_data> data{
		{.s = "hello", .i = 1, .name = {"B"}},
		{.s = "hi", .i = 0, .name = {"A"}}
	};

	const std::vector<entity_data> sorted_data = [this] {
		auto ret = data;
		std::ranges::sort(ret, [](const auto & lhs, const auto & rhs) { return lhs.i < rhs.i; });
		return ret;
	}();

	core_sort() {
		for (const auto & d: data) {
			const auto e = r.create();
			r.emplace<std::string>(e, d.s);
			r.emplace<int>(e, d.i);
			r.emplace<kengine::core::name>(e, d.name);
		}
	}
};
