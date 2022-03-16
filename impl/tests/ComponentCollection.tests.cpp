#include "tests/KengineTest.hpp"
#include "kengine.hpp"

struct ComponentCollection : KengineTest {};

struct EntityData {
    int i = 0;
    std::string s;
};

TEST_F(ComponentCollection, findZero) {
    const kengine::impl::ComponentCollection<int, std::string> collection;
    EXPECT_EQ(collection.size(), 0);

    for (const auto & [e, i, s] : collection)
        FAIL();
}

TEST_F(ComponentCollection, findOne) {
    const EntityData data {
        .i = 42,
        .s = "hello"
    };

    kengine::entities += [&](kengine::Entity & e) {
        e += data.i;
        e += data.s;
    };

    const kengine::impl::ComponentCollection<int, std::string> collection;
    EXPECT_EQ(collection.size(), 1);

    bool first = true;
    for (const auto & [e, i, s] : collection) {
        EXPECT_TRUE(first);
        first = false;
        EXPECT_EQ(i, data.i);
        EXPECT_EQ(s, data.s);
    }

    EXPECT_FALSE(first); // we should have found one
}

TEST_F(ComponentCollection, findMultiple) {
    const std::vector<EntityData> data {
        { .i = 42, .s = "hello" },
        { .i = 84, .s = "hi" }
    };

    for (const auto & d : data)
        kengine::entities += [&](kengine::Entity & e) {
            e += d.i;
            e += d.s;
        };

    const kengine::impl::ComponentCollection<int, std::string> collection;
    EXPECT_EQ(collection.size(), data.size());

    size_t count = 0;
    for (const auto & [e, i, s] : collection) {
        EXPECT_EQ(i, data[count].i);
        EXPECT_EQ(s, data[count].s);
        ++count;
    }
    EXPECT_EQ(count, data.size());
}

TEST_F(ComponentCollection, findMultipleArchetypes) {
    const std::vector<EntityData> data {
        { .i = 42, .s = "hello" },
        { .i = 84, .s = "hi" },
        { .i = -42, .s = "foo" },
        { .i = -84, .s = "bar" }
    };

    for (const auto & d : data)
        kengine::entities += [&](kengine::Entity & e) {
            e += d.i;
            e += d.s;
            if (d.i < 0)
                e += float(d.i * .5f); // also add a few with floats for another archetype
        };

    const kengine::impl::ComponentCollection<int, std::string> collection;
    EXPECT_EQ(collection.size(), data.size());

    size_t count = 0;
    for (const auto & [e, i, s] : collection) {
        EXPECT_EQ(i, data[count].i);
        EXPECT_EQ(s, data[count].s);
        ++count;
    }
    EXPECT_EQ(count, data.size());
}

TEST_F(ComponentCollection, findNot) {
    const std::vector<EntityData> data {
            { .i = 42, .s = "hello" },
            { .i = 84, .s = "hi" },
            { .i = -42, .s = "foo" },
            { .i = -84, .s = "bar" }
    };

    for (const auto & d : data)
        kengine::entities += [&](kengine::Entity & e) {
            e += d.i;
            e += d.s;
            if (d.i < 0)
                e += float(d.i * .5f); // also add a few with floats for another archetype
        };

    const kengine::impl::ComponentCollection<int, std::string, kengine::no<float>> collection;
    EXPECT_EQ(collection.size(), 2); // shouldn't find floats

    size_t count = 0;
    for (const auto & [e, i, s, noFloat] : collection) {
        EXPECT_EQ(i, data[count].i);
        EXPECT_EQ(s, data[count].s);
        ++count;
    }
    EXPECT_EQ(count, 2);
}
