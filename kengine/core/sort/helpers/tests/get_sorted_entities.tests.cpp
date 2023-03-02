#include "sort.tests.hpp"

// kengine core
#include "kengine/core/sort/helpers/get_sorted_entities.hpp"

TEST_F(core_sort, get_sorted_entities_std_vector) {
	const auto vec = kengine::core::sort::get_sorted_entities<const int, const std::string>(
		r, [](const auto & lhs, const auto & rhs) {
			return *std::get<1>(lhs) < *std::get<1>(rhs);
		}
	);

	static_assert(putils::specialization<putils_typeof(vec), std::vector>);
	EXPECT_EQ(vec.size(), data.size());

	size_t count = 0;
	for (const auto & [e, i, s]: vec) {
		EXPECT_EQ(*s, sorted_data[count].s);
		EXPECT_EQ(*i, sorted_data[count].i);
		++count;
	}
}

TEST_F(core_sort, get_sorted_entities_putils_vector) {
	const auto vec = kengine::core::sort::get_sorted_entities<3, const int, const std::string>(
		r, [](const auto & lhs, const auto & rhs) {
			return *std::get<1>(lhs) < *std::get<1>(rhs);
		}
	);

	static_assert(putils::is_vector<putils_typeof(vec) >());
	EXPECT_EQ(vec.size(), data.size());

	size_t count = 0;
	for (const auto & [e, i, s]: vec) {
		EXPECT_EQ(*s, sorted_data[count].s);
		EXPECT_EQ(*i, sorted_data[count].i);
		++count;
	}
}