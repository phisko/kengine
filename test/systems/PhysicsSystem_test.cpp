#include "SystemTest.hpp"
#include "common/systems/PhysicsSystem.hpp"
#include "common/gameobjects/KinematicObject.hpp"

struct PhysicsSystemTest : SystemTest<kengine::PhysicsSystem> {
};

TEST_F(PhysicsSystemTest, Movement) {
    const auto & go = em.createEntity<kengine::KinematicObject>(
            "bite",
            [](kengine::GameObject & go) {
                auto & movement = go.getComponent<kengine::PhysicsComponent>().movement;
                movement.x = 1;
            }
    );

    runTimes(1);

    const auto & pos = go.getComponent<kengine::TransformComponent3d>().boundingBox.topLeft;
    EXPECT_GE(pos.x, 1);
    EXPECT_LE(pos.x, 1.5);
    EXPECT_EQ(pos.y, 0);
    EXPECT_EQ(pos.z, 0);
}

TEST_F(PhysicsSystemTest, Continue) {
    const auto & go = em.createEntity<kengine::KinematicObject>(
            [](kengine::GameObject & go) {
                auto & movement = go.getComponent<kengine::PhysicsComponent>().movement;
                movement.x = 1;
            }
    );

    runTimes(2);

    const auto & pos = go.getComponent<kengine::TransformComponent3d>().boundingBox.topLeft;
    EXPECT_GE(pos.x, 2);
    EXPECT_LE(pos.x, 2.5);
    EXPECT_EQ(pos.y, 0);
    EXPECT_EQ(pos.z, 0);
}

TEST_F(PhysicsSystemTest, Stop) {
    auto & go = em.createEntity<kengine::KinematicObject>(
            [](kengine::GameObject & go) {
                auto & movement = go.getComponent<kengine::PhysicsComponent>().movement;
                movement.x = 1;
            }
    );

    runTimes(1);
    go.getComponent<kengine::PhysicsComponent>().movement.x = 0;
    runTimes(1);

    const auto & pos = go.getComponent<kengine::TransformComponent3d>().boundingBox.topLeft;
    EXPECT_GE(pos.x, 1);
    EXPECT_LE(pos.x, 1.5);
    EXPECT_EQ(pos.y, 0);
    EXPECT_EQ(pos.z, 0);
}

TEST_F(PhysicsSystemTest, Fixed) {
    const auto & go = em.createEntity<kengine::KinematicObject>(
            [](kengine::GameObject & go) {
                auto & phys = go.getComponent<kengine::PhysicsComponent>();
                phys.fixed = true;
                phys.movement.x = 10;
            }
    );

    runTimes(1);

    const auto & pos = go.getComponent<kengine::TransformComponent3d>().boundingBox.topLeft;
    EXPECT_EQ(pos.x, 0);
    EXPECT_EQ(pos.y, 0);
    EXPECT_EQ(pos.z, 0);
}

TEST_F(PhysicsSystemTest, Obstacle) {
    struct CollisionHandler : kengine::System<CollisionHandler, kengine::packets::Collision> {
        void handle(const kengine::packets::Collision & p) {
            ++received;
        }

        size_t received = 0;
    };
    const auto & handler = em.createSystem<CollisionHandler>();

    const auto & go = em.createEntity<kengine::KinematicObject>(
            [](kengine::GameObject & go) {
                auto & phys = go.getComponent<kengine::PhysicsComponent>();
                phys.movement.x = 1;
            }
    );
    em.createEntity<kengine::KinematicObject>(
            [](kengine::GameObject & go) {
                auto & box = go.getComponent<kengine::TransformComponent3d>().boundingBox;
                box.topLeft.x = 1;
            }
    );

    runTimes(1);

    const auto & pos = go.getComponent<kengine::TransformComponent3d>().boundingBox.topLeft;
    EXPECT_GT(pos.x, 1);
    EXPECT_LT(pos.x, 1.5);
    EXPECT_EQ(pos.y, 0);
    EXPECT_EQ(pos.z, 0);

    EXPECT_EQ(handler.received, 1);
}

TEST_F(PhysicsSystemTest, Query) {
    struct GetObjects : kengine::System<GetObjects> {
        void execute() final {
            response = query<kengine::packets::Position::Response>(
                    kengine::packets::Position::Query { { { 0, 0, 0 }, { 1, 1, 1 } } }
            );
        }
        kengine::packets::Position::Response response;
    };
    auto & s = em.createSystem<GetObjects>();
    const auto & go = em.createEntity<kengine::KinematicObject>();

    runTimes(1);

    const auto & objects = s.response.objects;
    ASSERT_EQ(objects.size(), 1);
    EXPECT_EQ(objects[0], &go);
}

TEST_F(PhysicsSystemTest, QueryEmpty) {
    struct GetObjects : kengine::System<GetObjects> {
        void execute() final {
            response = query<kengine::packets::Position::Response>(
                    kengine::packets::Position::Query { { { 0, 0, 0 }, { 1, 0, 1 } } }
            );
        }
        kengine::packets::Position::Response response;
    };
    auto & s = em.createSystem<GetObjects>();
    const auto & go = em.createEntity<kengine::KinematicObject>(
            [](kengine::GameObject & go) {
                auto & box = go.getComponent<kengine::TransformComponent3d>().boundingBox;
                box.topLeft.x = 42;
            }
    );

    runTimes(1);

    const auto & objects = s.response.objects;
    EXPECT_EQ(objects.size(), 0);
}
