#include "tests/KengineTest.hpp"
#include "helpers/sortHelper.hpp"

struct sortHelper : KengineTest {
    struct EntityData {
        std::string s;
        int i = 0;
        kengine::NameComponent name;
    };

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
            kengine::entities += [&](kengine::Entity & e) {
                e += d.s;
                e += d.i;
                e += d.name;
            };
        }
    }
};

TEST_F(sortHelper, getSortedEntities_StdVector) {
    const auto vec = kengine::sortHelper::getSortedEntities<int, std::string>([](const auto & lhs, const auto & rhs) {
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
    const auto vec = kengine::sortHelper::getSortedEntities<3, int, std::string>([](const auto & lhs, const auto & rhs) {
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
    const auto vec = kengine::sortHelper::getNameSortedEntities<int, std::string>();

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
    const auto vec = kengine::sortHelper::getNameSortedEntities<3, int, std::string>();

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
