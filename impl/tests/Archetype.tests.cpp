#include "tests/KengineTest.hpp"

// kengine impl
#include "impl/Archetype.hpp"

struct Archetype : KengineTest {
    kengine::impl::Archetype a;
};

TEST_F(Archetype, add) {
    a.add(0);
    EXPECT_EQ(a.entities.size(), 1);
    EXPECT_NE(std::ranges::find(a.entities, 0), a.entities.end());
}

TEST_F(Archetype, remove) {
    a.add(0);
    a.remove(0);
    EXPECT_EQ(a.entities.size(), 0);
    EXPECT_EQ(std::ranges::find(a.entities, 0), a.entities.end());
}

TEST_F(Archetype, sort) {
    a.add(1);
    a.add(0);
    a.sort();
    EXPECT_TRUE(a.sorted);
    EXPECT_EQ(a.entities[0], 0);
    EXPECT_EQ(a.entities[1], 1);
}

TEST_F(Archetype, matchesEmpty) {
    a.mask[kengine::impl::Component<int>::id()] = true;
    a.mask[kengine::impl::Component<std::string>::id()] = true;

    bool bMatches = a.matches<int, std::string>();
    EXPECT_FALSE(bMatches); // Shouldn't match when there aren't any entities
}

TEST_F(Archetype, matches) {
    a.mask[kengine::impl::Component<int>::id()] = true;
    a.mask[kengine::impl::Component<std::string>::id()] = true;

    a.add(0);

    bool bMatches = a.matches<int, std::string>();
    EXPECT_TRUE(bMatches);
    EXPECT_TRUE(a.matches<int>());
    EXPECT_TRUE(a.matches<std::string>());

    EXPECT_FALSE(a.matches<float>());
    bMatches = a.matches<int, float>();
    EXPECT_FALSE(bMatches);
    bMatches = a.matches<float, std::string>();
    EXPECT_FALSE(bMatches);
}

TEST_F(Archetype, matchesNot) {
    a.mask[kengine::impl::Component<int>::id()] = true;
    a.mask[kengine::impl::Component<std::string>::id()] = true;

    a.add(0);

    EXPECT_FALSE(a.matches<kengine::no<int>>());

    bool bMatches = a.matches<int, kengine::no<float>>();
    EXPECT_TRUE(bMatches);

    bMatches = a.matches<int, kengine::no<std::string>>();
    EXPECT_FALSE(bMatches);
}