#include <map>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "BulletSystem.hpp"
#include "EntityManager.hpp"

#include "components/GraphicsComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/PhysicsComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/DebugGraphicsComponent.hpp"
#include "components/ModelColliderComponent.hpp"
#include "components/SkeletonComponent.hpp"
#include "components/ModelComponent.hpp"

#include "helpers/MatrixHelper.hpp"
#include "helpers/SkeletonHelper.hpp"

#include "termcolor.hpp"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }
static putils::Point3f toPutils(const btVector3 & vec) { return { vec.getX(), vec.getY(), vec.getZ() }; }
static btVector3 toBullet(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

static btTransform toBullet(const kengine::TransformComponent3f & transform) {
	glm::mat4 mat(1.f);

	mat = glm::translate(mat, toVec(transform.boundingBox.position));
	mat = glm::rotate(mat, transform.pitch, { 1.f, 0.f, 0.f });
	mat = glm::rotate(mat, transform.yaw, { 0.f, 1.f, 0.f });
	mat = glm::rotate(mat, transform.roll, { 0.f, 0.f, 1.f });

	btTransform ret;
	ret.setFromOpenGLMatrix(glm::value_ptr(mat));

	return ret;
}

static btTransform toBullet(const kengine::ModelColliderComponent::Collider & collider, const kengine::SkeletonComponent * skeleton, const kengine::ModelSkeletonComponent * modelSkeleton, const kengine::ModelComponent * model) {
	glm::mat4 mat(1.f);

	if (!collider.boneName.empty()) {
		assert(skeleton != nullptr && modelSkeleton != nullptr && model != nullptr);

		mat = glm::scale(mat, { -1.f, 1.f, -1.f });
		const auto worldSpaceBone = kengine::SkeletonHelper::getBoneMatrix(collider.boneName.c_str(), *skeleton, *modelSkeleton);
		const auto pos = kengine::MatrixHelper::getPos(worldSpaceBone);
		mat = glm::translate(mat, toVec(pos * model->boundingBox.size));
		mat = glm::translate(mat, -toVec(pos));
		mat *= worldSpaceBone;
	}

	mat = glm::translate(mat, toVec(collider.boundingBox.position));
	mat = glm::rotate(mat, collider.pitch, { 1.f, 0.f, 0.f });
	mat = glm::rotate(mat, collider.yaw, { 0.f, 1.f, 0.f });
	mat = glm::rotate(mat, collider.roll, { 0.f, 0.f, 1.f });

	btTransform ret;
	ret.setFromOpenGLMatrix(glm::value_ptr(mat));

	return ret;
}

static auto ENABLE_DEBUG = false;
static auto GRAVITY = 1.f;

static btDefaultCollisionConfiguration collisionConfiguration;
static btCollisionDispatcher dispatcher(&collisionConfiguration);
static btDbvtBroadphase overlappingPairCache;
static btSequentialImpulseConstraintSolver solver;

static btDiscreteDynamicsWorld dynamicsWorld(&dispatcher, &overlappingPairCache, &solver, &collisionConfiguration);

static kengine::EntityManager * g_em = nullptr;
void detectCollisions(btDynamicsWorld *, btScalar timeStep) {
	const auto numManifolds = dispatcher.getNumManifolds();
	for (int i = 0; i < numManifolds; ++i) {
		const auto contactManifold = dispatcher.getManifoldByIndexInternal(i);
		const auto objectA = (btCollisionObject*)(contactManifold->getBody0());
		const auto objectB = (btCollisionObject*)(contactManifold->getBody1());

		const auto id1 = objectA->getUserIndex();
		const auto id2 = objectB->getUserIndex();
		auto e1 = g_em->getEntity(id1);
		auto e2 = g_em->getEntity(id2);
		g_em->send(kengine::packets::Collision{ e1, e2 });
	}
}

struct BulletPhysicsComponent : kengine::not_serializable {
	btCompoundShape * shape;
	btRigidBody * body;
	pmeta_get_class_name(BulletPhysicsComponent);
};

struct KengineMotionState : public btMotionState {
	KengineMotionState(kengine::TransformComponent3f & transform) : transform(transform) {}

	void getWorldTransform(btTransform & worldTrans) const final {
		worldTrans = toBullet(transform);
	}

	void setWorldTransform(const btTransform & worldTrans) final {
		transform.boundingBox.position = toPutils(worldTrans.getOrigin());

		btScalar xRotation, yRotation, zRotation;
		worldTrans.getRotation().getEulerZYX(zRotation, yRotation, xRotation);

		transform.pitch = xRotation;
		transform.yaw = yRotation;
		transform.roll = zRotation;
	}

	kengine::TransformComponent3f & transform;
};

#ifndef KENGINE_NDEBUG
namespace debug {
	class Drawer : public btIDebugDraw {
	public:
		Drawer(kengine::EntityManager & em) : _em(em) {}

		void cleanup() {
			for (const auto id : _toCleanup)
				_em.removeEntity(id);
			_toCleanup.clear();
		}

		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override {
			_em += [&](kengine::Entity & e) {
				const auto a = toPutils(from);
				const auto b = toPutils(to) - a;
				e += kengine::TransformComponent3f({ a });
				e += kengine::DebugGraphicsComponent(kengine::DebugGraphicsComponent::Line, { b }, putils::NormalizedColor{ color[0], color[1], color[2], 1.f });
				_toCleanup.push_back(e.id);
			};
		}

		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {
		}

		void draw3dText(const btVector3& location, const char* textString) override {}

		void reportErrorWarning(const char* warningString) override { std::cout << putils::termcolor::red << "[Bullet] " << warningString << putils::termcolor::reset; }

		void setDebugMode(int debugMode) override {}
		int getDebugMode() const override { return DBG_DrawWireframe; }

	private:
		kengine::EntityManager & _em;
		std::vector<kengine::Entity::ID> _toCleanup;
	};

	static Drawer * drawer = nullptr;
}
#endif

namespace putils {
	inline bool operator<(const Point3f & lhs, const Point3f & rhs) {
		if (lhs.x < rhs.x) return true;
		if (lhs.x > rhs.x) return false;
		if (lhs.y < rhs.y) return true;
		if (lhs.y > rhs.y) return false;
		if (lhs.z < rhs.z) return true;
		if (lhs.z > rhs.z) return false;
		return false;
	}
}

namespace kengine {
	BulletSystem::BulletSystem(EntityManager & em)
		: System(em), _em(em)
	{
		onLoad("");

#ifndef KENGINE_NDEBUG
		debug::drawer = new debug::Drawer(em);
#endif

		g_em = &em;
		// dynamicsWorld.setInternalTickCallback(detectCollisions);
	}

	void BulletSystem::onLoad(const char * directory) noexcept {
		_em += [](Entity & e) { e += AdjustableComponent("[Physics] Gravity", &GRAVITY); };
#ifndef KENGINE_NDEBUG
		_em += [](Entity & e) { e += AdjustableComponent("[Physics] Debug", &ENABLE_DEBUG); };
#endif
	}

	using CollisionShapeMap = std::map<putils::Point3f, std::unique_ptr<btCollisionShape>>;

	template<typename Func>
	static btCollisionShape * getCollisionShape(CollisionShapeMap & shapes, const putils::Vector3f & size, Func && creator) {
		const auto it = shapes.find(size);
		if (it != shapes.end())
			return it->second.get();
		const auto ret = creator();
		ret->setLocalScaling({ 1.f, 1.f, 1.f });
		shapes.emplace(size, ret);
		return ret;
	}

	static void addBulletComponent(Entity & e, TransformComponent3f & transform, const PhysicsComponent & physics, const Entity & modelEntity) {
		auto & comp = e.attach<BulletPhysicsComponent>();

		const auto & modelCollider = modelEntity.get<ModelColliderComponent>();

		const SkeletonComponent * skeleton = e.has<SkeletonComponent>() ?
			&e.get<SkeletonComponent>() : nullptr;
		const ModelSkeletonComponent * modelSkeleton = modelEntity.has<ModelSkeletonComponent>() ?
			&modelEntity.get<ModelSkeletonComponent>() : nullptr;
		const ModelComponent * modelComponent = modelEntity.has<ModelComponent>() ?
			&modelEntity.get<ModelComponent>() : nullptr;

		comp.shape = new btCompoundShape(false);
		for (const auto & collider : modelCollider.colliders) {
			const auto size = transform.boundingBox.size * collider.boundingBox.size;

			btCollisionShape * shape; {
				switch (collider.shape) {
				case ModelColliderComponent::Collider::Box: {
					static CollisionShapeMap shapes;
					shape = getCollisionShape(shapes, size, [&] { return new btBoxShape({ size.x / 2.f, size.y / 2.f, size.z / 2.f }); });
					break;
				}
				case ModelColliderComponent::Collider::Capsule: {
					static CollisionShapeMap shapes;
					shape = getCollisionShape(shapes, size, [&] { return new btCapsuleShape(std::min(size.x, size.z) / 2.f, size.y); });
					break;
				}
				case ModelColliderComponent::Collider::Cone: {
					static CollisionShapeMap shapes;
					shape = getCollisionShape(shapes, size, [&] { return new btConeShape(std::min(size.x, size.z) / 2.f, size.y); });
					break;
				}
				case ModelColliderComponent::Collider::Cylinder: {
					static CollisionShapeMap shapes;
					shape = getCollisionShape(shapes, size, [&] { return new btCylinderShape({ size.x / 2.f, size.y / 2.f, size.z / 2.f }); });
					break;
				}
				case ModelColliderComponent::Collider::Sphere: {
					static CollisionShapeMap shapes;
					shape = getCollisionShape(shapes, size, [&] { return new btSphereShape(std::min(std::min(size.x, size.y), size.y) / 2.f); });
					break;
				}
				default:
					assert(!"Unknown collider shape");
					break;
				}
			}
			comp.shape->addChildShape(toBullet(collider, skeleton, modelSkeleton, modelComponent), shape);
		}

		btVector3 localInertia{ 0.f, 0.f, 0.f }; {
			if (physics.mass != 0.f)
				comp.shape->calculateLocalInertia(physics.mass, localInertia);
		}

		const auto motionState = new KengineMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(physics.mass, motionState, comp.shape, localInertia);
		// btRigidBody::btRigidBodyConstructionInfo rbInfo(physics.mass, motionState, mass != 0.f ? (btCollisionShape*)(new btSphereShape(.5f)) : (btCollisionShape*)(new btBoxShape({ 6.25, 0.125, 6.05 })), localInertia);
		comp.body = new btRigidBody(rbInfo);
		comp.body->setUserIndex((int)e.id);

		dynamicsWorld.addRigidBody(comp.body);
	}

	static void updateBulletComponent(Entity & e, const TransformComponent3f & transform, PhysicsComponent & physics, const Entity & modelEntity) {
		auto & comp = e.get<BulletPhysicsComponent>();

		if (physics.changed) {
			// comp.body->clearForces();
			comp.body->setLinearVelocity(toBullet(physics.movement * physics.speed));

			btVector3 localInertia{ 0.f, 0.f, 0.f };
			if (physics.mass != 0.f)
				comp.body->getCollisionShape()->calculateLocalInertia(physics.mass, localInertia);
			comp.body->setMassProps(physics.mass, localInertia);

			comp.body->setActivationState(ACTIVE_TAG);
			comp.body->setDeactivationTime(0.f);
		}
		else if (!physics.kinematic)
			physics.movement = toPutils(comp.body->getLinearVelocity()) / physics.speed;

		if (physics.kinematic) {
			comp.body->setCollisionFlags(comp.body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			comp.body->setActivationState(WANTS_DEACTIVATION);
		}
		else if (comp.body->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT) {
			comp.body->setCollisionFlags(comp.body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
			comp.body->setActivationState(ACTIVE_TAG);
			comp.body->setDeactivationTime(0.f);
		}

		physics.changed = false;

		if (e.has<SkeletonComponent>() && modelEntity.has<ModelSkeletonComponent>()) {
			const auto & skeleton = e.get<SkeletonComponent>();
			const auto & modelSkeleton = modelEntity.get<ModelSkeletonComponent>();
			const auto & modelComponent = modelEntity.get<ModelComponent>();

			int i = 0;
			for (const auto & collider : modelEntity.get<ModelColliderComponent>().colliders) {
				comp.shape->updateChildTransform(i, toBullet(collider, &skeleton, &modelSkeleton, &modelComponent));
				++i;
			}
		}
	}

	void BulletSystem::execute() {
		for (auto &[e, graphics, transform, physics] : _em.getEntities<GraphicsComponent, TransformComponent3f, PhysicsComponent>()) {
			if (graphics.model == Entity::INVALID_ID)
				continue;

			const auto & modelEntity = _em.getEntity(graphics.model);
			if (!modelEntity.has<ModelColliderComponent>())
				continue;
			if (!e.has<BulletPhysicsComponent>())
				addBulletComponent(e, transform, physics, modelEntity);
			else
				updateBulletComponent(e, transform, physics, modelEntity);
		}

		dynamicsWorld.setGravity({ 0.f, -GRAVITY, 0.f });
		dynamicsWorld.stepSimulation(time.getDeltaTime().count());

#ifndef KENGINE_NDEBUG
		debug::drawer->cleanup();
		dynamicsWorld.setDebugDrawer(ENABLE_DEBUG ? debug::drawer : nullptr);
		dynamicsWorld.debugDrawWorld();
#endif
	}

	void BulletSystem::handle(packets::RemoveEntity p) {
		if (!p.e.has<BulletPhysicsComponent>())
			return;
		auto & comp = p.e.get<BulletPhysicsComponent>();
		dynamicsWorld.removeRigidBody(comp.body);
		delete comp.body->getMotionState();
		delete comp.body;
		delete comp.shape;
	}

	void BulletSystem::handle(const packets::QueryPosition & p) {
		btSphereShape sphere(p.radius);
		btPairCachingGhostObject ghost;
		btTransform transform;
		transform.setOrigin(toBullet(p.pos));

		ghost.setCollisionShape(&sphere);
		ghost.setWorldTransform(transform);
		ghost.activate(true);

		struct Callback : btCollisionWorld::ContactResultCallback {
			Callback(btPairCachingGhostObject & ghost, packets::QueryPosition::Results & results) : ghost(ghost), results(results) {}

			bool needsCollision(btBroadphaseProxy* proxy0) const final {
				return !results.full() && btCollisionWorld::ContactResultCallback::needsCollision(proxy0);
			}

			btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) final {
				assert(colObj1Wrap->m_collisionObject == &ghost);
				const auto id = colObj0Wrap->m_collisionObject->getUserIndex();
				if (std::find(results.begin(), results.end(), id) == results.end())
					results.push_back(id);
				return 1.f;
			}

			btPairCachingGhostObject & ghost;
			packets::QueryPosition::Results & results;
		};

		Callback callback(ghost, p.results);
		dynamicsWorld.contactTest(&ghost, callback);
	}
}