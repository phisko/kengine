#include "tests/KengineTest.hpp"
#include "scoped_setter.hpp"

struct Entity : KengineTest {};

TEST_F(Entity, getMissing) {
    const auto e = kengine::entities += [](kengine::Entity & e) {};
	EXPECT_DEATH(e.get<int>(), ".*");
}

TEST_F(Entity, get) {
    const auto e = kengine::entities += [](kengine::Entity & e) {
        e += std::string("hello");
    };
    EXPECT_EQ(e.get<std::string>(), "hello");
}

TEST_F(Entity, has) {
    const auto e = kengine::entities += [](kengine::Entity & e) {
        EXPECT_FALSE(e.has<std::string>());
        e += std::string("hello");
        EXPECT_TRUE(e.has<std::string>());
    };
    EXPECT_FALSE(e.has<int>());
    EXPECT_TRUE(e.has<std::string>());
}

TEST_F(Entity, tryGet) {
    const auto e = kengine::entities += [](kengine::Entity & e) {
        EXPECT_EQ(e.tryGet<std::string>(), nullptr);
        e += std::string("hello");
        EXPECT_EQ(*e.tryGet<std::string>(), "hello");
    };
    EXPECT_EQ(e.tryGet<int>(), nullptr);
    EXPECT_EQ(*e.tryGet<std::string>(), "hello");
}

TEST_F(Entity, attachNothing) {
    kengine::entities += [](kengine::Entity & e) {
        e.attach<std::string>();
        EXPECT_EQ(e.get<std::string>(), "");
    };
}

TEST_F(Entity, attachSomething) {
    kengine::entities += [](kengine::Entity & e) {
        e.attach(std::string("hello"));
        EXPECT_EQ(e.get<std::string>(), "hello");
    };
}

TEST_F(Entity, attachOverwrite) {
    kengine::entities += [](kengine::Entity & e) {
        e.attach(std::string("hello"));
		e.attach(std::string("hey"));
        EXPECT_EQ(e.get<std::string>(), "hey");
    };
}

TEST_F(Entity, detachMissing) {
    auto e = kengine::entities += [](kengine::Entity & e) {};

    bool asserted = false;
    const auto _ = putils::setForScope(kengine::assertHelper::assertHandler, [&](auto && ...) {
        asserted = true;
        return false;
    });
    e.detach<int>();
    EXPECT_TRUE(asserted);
}

TEST_F(Entity, detach) {
    static bool done = false;
    struct Comp {
        ~Comp() {
            done = true;
        }
    };

    auto e = kengine::entities += [](kengine::Entity & e) {
        e += Comp{};
    };

    done = false;
    e.detach<Comp>();

    EXPECT_TRUE(done);
}
