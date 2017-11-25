#include "SystemTest.hpp"
#include "common/systems/PathfinderSystem.hpp"
#include "common/gameobjects/KinematicObject.hpp"

struct PathfinderSystemTest : SystemTest<kengine::PhysicsSystem, kengine::PathfinderSystem>
{
};

void expectReached(const kengine::GameObject &go, const putils::Point3d &dest)
{
    const auto &pos = go.getComponent<kengine::TransformComponent3d>().boundingBox.topLeft;
    EXPECT_GE(pos.x, dest.x);
    EXPECT_LE(pos.x, dest.x + 1);

    EXPECT_GE(pos.y, dest.y);
    EXPECT_LE(pos.y, dest.y + 1);

    EXPECT_GE(pos.z, dest.z);
    EXPECT_LE(pos.z, dest.z + 1);

    const auto &path = go.getComponent<kengine::PathfinderComponent>();
    EXPECT_TRUE(path.reached);
}

TEST_F(PathfinderSystemTest, Move)
{
    const auto &go = em.createEntity<kengine::KinematicObject>(
            [](kengine::GameObject &go)
            {
                auto &path = go.attachComponent<kengine::PathfinderComponent>();
                path.dest.x = 5;
                path.reached = false;
            }
    );

    runTimes(5);

    expectReached(go, { 4, 0, 0 });
}

TEST_F(PathfinderSystemTest, Move2d)
{
    const auto &go = em.createEntity<kengine::KinematicObject>(
            [](kengine::GameObject &go)
            {
                auto &path = go.attachComponent<kengine::PathfinderComponent>();
                path.dest = { 5, 0, 5 };
                path.reached = false;
            }
    );

    runTimes(10);

    expectReached(go, { 5, 0, 5 });
}

TEST_F(PathfinderSystemTest, Obstacle)
{
    struct CheckAvoids : kengine::System<CheckAvoids>
    {
        CheckAvoids(kengine::EntityManager &em) : em(em) {}
        kengine::EntityManager &em;

        void execute() final
        {
            for (const auto go : em.getGameObjects())
                for (const auto other : em.getGameObjects())
                    if (intersect(*go, *other))
                        good = false;
        }

        bool intersect(const kengine::GameObject &go, const kengine::GameObject &other)
        {
            const auto &box1 = go.getComponent<kengine::TransformComponent3d>().boundingBox;
            const auto &box2 = other.getComponent<kengine::TransformComponent3d>().boundingBox;
            return &go != &other && box1.intersect(box2);
        }

        bool good = true;
    };

    const auto &check = em.createSystem<CheckAvoids>(em);
    const auto &go = em.createEntity<kengine::KinematicObject>(
            [](kengine::GameObject &go)
            {
                auto &path = go.attachComponent<kengine::PathfinderComponent>();
                path.dest.x = 5;
                path.reached = false;
            }
    );
    em.createEntity<kengine::KinematicObject>(nullptr, putils::Rect3d{ { 1, 0, 0 } });

    runTimes(5);

    expectReached(go, { 4, 0, 0 });
    EXPECT_TRUE(check.good);
}
