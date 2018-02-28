#include "Box2DSystem.hpp"
#include "Box2DComponent.hpp"
#include "components/PhysicsComponent.hpp"
#include "components/TransformComponent.hpp"
#include "packets/Collision.hpp"

static const auto VELOCITY_ITERATIONS = 6;
static const auto POSITION_ITERATIONS = 2;

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) { return new kengine::Box2DSystem(em); }

static std::vector<std::pair<kengine::GameObject *, kengine::GameObject *>> contacts;

namespace kengine {
    Box2DSystem::Box2DSystem(kengine::EntityManager & em) : _em(em) {
        class ContactListener : public b2::ContactListener {
            void BeginContact(b2::Contact * contact) noexcept final {
                const auto first = contact->GetFixtureA()->GetBody()->GetUserData();
                const auto second = contact->GetFixtureB()->GetBody()->GetUserData();
                contacts.emplace_back((kengine::GameObject *)first, (kengine::GameObject *)second);
            }
        };
        static ContactListener contactListener;

        _world.SetContactListener(&contactListener);
    }

    void Box2DSystem::execute() noexcept {
        for (const auto go : _em.getGameObjects<Box2DComponent>())
            updateBody(*go);

        _world.Step((float)time.getDeltaFrames(), VELOCITY_ITERATIONS, POSITION_ITERATIONS);

        for (const auto go : _em.getGameObjects<Box2DComponent>())
            updateTransform(*go);

        handleCollisions();
    }

    void Box2DSystem::updateBody(kengine::GameObject & go) noexcept {
        const auto & phys = go.getComponent<PhysicsComponent>();
        auto & comp = go.getComponent<Box2DComponent>();
        comp.body->SetLinearVelocity(
                { (float)(phys.movement.x * phys.speed), (float)(phys.movement.z * phys.speed) }
        );

        const auto & transform = go.getComponent<TransformComponent3d>();
        const auto & box = transform.boundingBox;
        const auto & pos = box.topLeft;
        const auto & size = box.size;
        comp.body->SetTransform({ (float)pos.x, (float)pos.z }, (float)transform.yaw);

        comp.body->DestroyFixture(comp.body->GetFixtureList());

        b2::PolygonShape dynamicBox;
        dynamicBox.SetAsBox((float)box.size.x / 2, (float)box.size.z / 2);

        b2::FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 1.0f; // TODO: play with this
        comp.body->CreateFixture(&fixtureDef);
    }

    void Box2DSystem::updateTransform(kengine::GameObject & go) noexcept {
        const auto & comp = go.getComponent<Box2DComponent>();
        auto & box = go.getComponent<kengine::TransformComponent3d>().boundingBox;
        const auto & position = comp.body->GetPosition();
        box.topLeft.x = position.x;
        box.topLeft.z = position.y;
    }

    void Box2DSystem::handleCollisions() noexcept {
        for (const auto & p : contacts)
            send(kengine::packets::Collision{ *p.first, *p.second });
        contacts.clear();
    }

    void Box2DSystem::handle(const kengine::packets::RegisterGameObject & p) noexcept  {
        auto & go = p.go;
        if (!go.hasComponent<kengine::TransformComponent3d>() || !go.hasComponent<kengine::PhysicsComponent>())
            return;

        const auto & box = go.getComponent<kengine::TransformComponent3d>().boundingBox;

        b2::BodyDef bodyDef;
        bodyDef.type = b2::dynamicBody;
        bodyDef.userData = &go;

        const auto body = _world.CreateBody(&bodyDef);
        go.attachComponent<Box2DComponent>().body = body;

        b2::PolygonShape dynamicBox;
        dynamicBox.SetAsBox((float)box.size.x / 2, (float)box.size.z / 2);

        b2::FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 1.0f; // TODO: play with this
        body->CreateFixture(&fixtureDef);
    }

    void Box2DSystem::handle(const kengine::packets::RemoveGameObject & p) noexcept  {
        auto & go = p.go;
        if (go.hasComponent<Box2DComponent>())
            _world.DestroyBody(go.getComponent<Box2DComponent>().body);
    }

    void Box2DSystem::handle(const packets::Position::Query & q) noexcept  {
        struct Callback : public b2::QueryCallback {
            bool ReportFixture(b2::Fixture * fixture) noexcept final {
                objects.push_back((GameObject *)fixture->GetBody()->GetUserData());
                return true;
            }

            std::vector<GameObject *> objects;
        };

        Callback callback;
        _world.QueryAABB(
                &callback,
                b2::AABB{
                        { (float)q.box.topLeft.x, (float)q.box.topLeft.z },
                        { (float)(q.box.topLeft.x + q.box.size.x), (float)(q.box.topLeft.z + q.box.size.z) },
                }
        );

        sendTo(packets::Position::Response { std::move(callback.objects) }, *q.sender);
    }
}