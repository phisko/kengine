#include "tests/KengineTest.hpp"
#include "impl/Component.hpp"

struct Component : KengineTest {};

TEST_F(Component, get) {
    auto & i = kengine::impl::Component<int>::get(0);
    i = 42;
    EXPECT_EQ(kengine::impl::Component<int>::get(0), 42);
}

TEST_F(Component, id) {
    EXPECT_EQ(kengine::impl::Component<int>::id(), 0);
    EXPECT_EQ(kengine::impl::Component<std::string>::id(), 1);
    EXPECT_EQ(kengine::impl::Component<int>::id(), 0);
    EXPECT_EQ(kengine::impl::Component<float>::id(), 2);
}

TEST_F(Component, dtor) {
    static int dtorCount = 0;
    struct TestComponent {
        std::optional<int> value;

        ~TestComponent() noexcept {
            if (value)
                ++dtorCount;
        }

        TestComponent(int value) noexcept
        : value(value)
        {}

        TestComponent() noexcept = default;
        TestComponent(TestComponent &&) noexcept = default;
        TestComponent & operator=(TestComponent &&) noexcept = default;
    };

    auto & v = kengine::impl::Component<TestComponent>::set(0);
    v.value = 42;
    kengine::impl::Component<TestComponent>::metadata().reset(0);
    EXPECT_EQ(dtorCount, 1);

    {
        TestComponent source{ 42 };
        kengine::impl::Component<TestComponent>::set(1, std::move(source));
        EXPECT_EQ(dtorCount, 1);
    }

    EXPECT_EQ(dtorCount, 2);
}
