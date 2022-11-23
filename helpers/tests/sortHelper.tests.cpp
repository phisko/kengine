// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine helpers
#include "helpers/sortHelper.hpp"

struct sortHelper : ::testing::Test {
    struct EntityData {
        std::string s;
        int i = 0;
        kengine::NameComponent name;
    };

	entt::registry r;

    const std::vector<EntityData> data {
        { .s = "hello", .i = 1, .name = { "B" } },
        { .s = "hi", .i = 0, .name = { "A" } }
    };

    const std::vector<EntityData> sortedData = [this] {
        auto ret = data;
        std::ranges::sort(ret, [](const auto & lhs, const auto & rhs) { return lhs.i < rhs.i; });
        return ret;
    }();

    sortHelper() {
		for (const auto & d : data) {
			const auto e = r.create();
			r.emplace<std::string>(e, d.s);
			r.emplace<int>(e, d.i);
			r.emplace<kengine::NameComponent>(e, d.name);
		}
    }
};

TEST_F(sortHelper, getSortedEntities_StdVector) {
    const auto vec = kengine::sortHelper::getSortedEntities<const int, const std::string>(r, [](const auto & lhs, const auto & rhs) {
        return *std::get<1>(lhs) < *std::get<1>(rhs);
    });

    static_assert(putils::is_specialization<putils_typeof(vec), std::vector>());
    EXPECT_EQ(vec.size(), data.size());

    size_t count = 0;
    for (const auto & [e, i, s] : vec) {
        EXPECT_EQ(*s, sortedData[count].s);
        EXPECT_EQ(*i, sortedData[count].i);
        ++count;
    }
}

TEST_F(sortHelper, getSortedEntities_PutilsVector) {
    const auto vec = kengine::sortHelper::getSortedEntities<3, const int, const std::string>(r, [](const auto & lhs, const auto & rhs) {
        return *std::get<1>(lhs) < *std::get<1>(rhs);
    });

    static_assert(putils::is_vector<putils_typeof(vec)>());
    EXPECT_EQ(vec.size(), data.size());

    size_t count = 0;
    for (const auto & [e, i, s] : vec) {
        EXPECT_EQ(*s, sortedData[count].s);
        EXPECT_EQ(*i, sortedData[count].i);
        ++count;
    }
}

TEST_F(sortHelper, getNameSortedEntities_StdVector) {
    const auto vec = kengine::sortHelper::getNameSortedEntities<const int, const std::string>(r);

    static_assert(putils::is_specialization<putils_typeof(vec), std::vector>());
    EXPECT_EQ(vec.size(), data.size());

    size_t count = 0;
    for (const auto & [e, name, i, s] : vec) {
        EXPECT_EQ(name->name, sortedData[count].name.name);
        EXPECT_EQ(*s, sortedData[count].s);
        EXPECT_EQ(*i, sortedData[count].i);
        ++count;
    }
}

TEST_F(sortHelper, getNameSortedEntities_PutilsVector) {
    const auto vec = kengine::sortHelper::getNameSortedEntities<3, const int, const std::string>(r);

    static_assert(putils::is_vector<putils_typeof(vec)>());
    EXPECT_EQ(vec.size(), data.size());

    size_t count = 0;
    for (const auto & [e, name, i, s] : vec) {
        EXPECT_EQ(name->name, sortedData[count].name.name);
        EXPECT_EQ(*s, sortedData[count].s);
        EXPECT_EQ(*i, sortedData[count].i);
        ++count;
    }
}
