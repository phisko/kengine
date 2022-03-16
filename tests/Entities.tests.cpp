#include "tests/KengineTest.hpp"
#include "Entities.hpp"
#include "RAII.hpp"

struct Entities : KengineTest {};

TEST_F(Entities, create) {
    bool done = false;
    kengine::entities.create([&](kengine::Entity & e) {
        done = true;
    });
    EXPECT_TRUE(done);
}

TEST_F(Entities, operatorCreate) {
    bool done = false;
    kengine::entities += [&](kengine::Entity & e) {
        done = true;
    };
    EXPECT_TRUE(done);
}

struct RemoveTestComponent {
    static void increment(size_t * & i) noexcept {
        ++*i;
    }
    using DTorCounter = putils::RAII<size_t *, increment>;
    DTorCounter dtorCounter;
};

TEST_F(Entities, remove) {
    size_t dtor = 0;
    const auto e = kengine::entities.create([&](kengine::Entity & e) {
        e += RemoveTestComponent{ RemoveTestComponent::DTorCounter{ &dtor } };
    });
    kengine::entities.remove(e);
    EXPECT_EQ(dtor, 1);
}

TEST_F(Entities, removeID) {
    size_t dtor = 0;
    const auto e = kengine::entities.create([&](kengine::Entity & e) {
        e += RemoveTestComponent{ RemoveTestComponent::DTorCounter{ &dtor } };
    });
    kengine::entities.remove(e.id);
    EXPECT_EQ(dtor, 1);
}

TEST_F(Entities, operatorRemove) {
    size_t dtor = 0;
    const auto e = kengine::entities.create([&](kengine::Entity & e) {
        e += RemoveTestComponent{ RemoveTestComponent::DTorCounter{ &dtor } };
    });
    kengine::entities -= e;
    EXPECT_EQ(dtor, 1);
}

TEST_F(Entities, operatorRemoveID) {
    size_t dtor = 0;
    const auto e = kengine::entities.create([&](kengine::Entity & e) {
        e += RemoveTestComponent{ RemoveTestComponent::DTorCounter{ &dtor } };
    });
    kengine::entities -= e.id;
    EXPECT_EQ(dtor, 1);
}

TEST_F(Entities, get) {
    const auto created = kengine::entities += [](kengine::Entity & e) {
        e += std::string("hello");
    };

    const auto e = kengine::entities.get(created.id);
    EXPECT_EQ(e.id, created.id);
    EXPECT_EQ(e.componentMask, created.componentMask);
    EXPECT_EQ(e.get<std::string>(), "hello");
}

TEST_F(Entities, operatorGet) {
    const auto created = kengine::entities += [](kengine::Entity & e) {
        e += std::string("hello");
    };

    const auto e = kengine::entities[created.id];
    EXPECT_EQ(e.id, created.id);
    EXPECT_EQ(e.componentMask, created.componentMask);
    EXPECT_EQ(e.get<std::string>(), "hello");
}

TEST_F(Entities, setActive) {
    const auto created = kengine::entities += [](kengine::Entity & e) {
        e += std::string("hello");
    };

    const auto collection = kengine::entities.with<std::string>();
    EXPECT_EQ(collection.size(), 1);
    kengine::entities.setActive(created, false);
    EXPECT_EQ(collection.size(), 0);
}

TEST_F(Entities, setActiveID) {
    const auto created = kengine::entities += [](kengine::Entity & e) {
        e += std::string("hello");
    };

    const auto collection = kengine::entities.with<std::string>();
    EXPECT_EQ(collection.size(), 1);
    kengine::entities.setActive(created.id, false);
    EXPECT_EQ(collection.size(), 0);
}

TEST_F(Entities, iterate) {
    const std::vector<std::string> strings = {
        "hello",
        "hi",
        "foo",
        "bar"
    };

    for (const auto & s : strings) {
        kengine::entities += [&](kengine::Entity & e) {
            e += s;
        };
    }

    EXPECT_EQ(kengine::entities.size(), strings.size());

    size_t count = 0;
    for (const auto e : kengine::entities) {
        EXPECT_EQ(e.get<std::string>(), strings[count]);
        ++count;
    }
    EXPECT_EQ(count, strings.size());
}