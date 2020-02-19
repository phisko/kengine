#include <map>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "BulletSystem.hpp"
#include "EntityManager.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/DebugGraphicsComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/KinematicComponent.hpp"
#include "data/ModelColliderComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/ModelSkeletonComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/SkeletonComponent.hpp"
#include "data/TransformComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnEntityRemoved.hpp"
#include "functions/OnCollision.hpp"
#include "functions/QueryPosition.hpp"

#include "helpers/MatrixHelper.hpp"
#include "helpers/SkeletonHelper.hpp"

#include "termcolor.hpp"
#include "magic_enum.hpp"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

struct BulletPhysicsComponent {
	btCompoundShape * shape;
	btRigidBody * body;
};

static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }
static putils::Point3f toPutils(const btVector3 & vec) { return { vec.getX(), vec.getY(), vec.getZ() }; }
static btVector3 toBullet(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

static btTransform toBullet(const kengine::TransformComponent & transform) {
	glm::mat4 mat(1.f);

	mat = glm::translate(mat, toVec(transform.boundingBox.position));
	mat = glm::rotate(mat, transform.pitch, { 1.f, 0.f, 0.f });
	mat = glm::rotate(mat, transform.yaw, { 0.f, 1.f, 0.f });
	mat = glm::rotate(mat, transform.roll, { 0.f, 0.f, 1.f });
	mat = glm::scale(mat, toVec(transform.boundingBox.size));

	btTransform ret;
	ret.setFromOpenGLMatrix(glm::value_ptr(mat));

	return ret;
}

static btTransform toBullet(const kengine::TransformComponent & parent, const kengine::ModelColliderComponent::Collider & collider, const kengine::SkeletonComponent * skeleton, const kengine::ModelSkeletonComponent * modelSkeleton, const kengine::ModelComponent * model) {
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

	mat = glm::rotate(mat, parent.pitch, { 1.f, 0.f, 0.f });
	mat = glm::rotate(mat, parent.yaw, { 0.f, 1.f, 0.f });
	mat = glm::rotate(mat, parent.roll, { 0.f, 0.f, 1.f });
	if (collider.boneName.empty()) // Otherwise, scale was embedded in bone matrix
		mat = glm::scale(mat, toVec(parent.boundingBox.size));

	mat = glm::translate(mat, toVec(collider.boundingBox.position));
	mat = glm::rotate(mat, collider.pitch, { 1.f, 0.f, 0.f });
	mat = glm::rotate(mat, collider.yaw, { 0.f, 1.f, 0.f });
	mat = glm::rotate(mat, collider.roll, { 0.f, 0.f, 1.f });

	btTransform ret;
	ret.setFromOpenGLMatrix(glm::value_ptr(mat));

	return ret;
}

#ifndef KENGINE_NDEBUG
namespace debug {
	class Drawer : public btIDebugDraw {
	public:
		Drawer(kengine::EntityManager & em) : _em(em) {
			em += [this](kengine::Entity & e) {
				e += kengine::TransformComponent{};
				e += kengine::DebugGraphicsComponent{};
				_debugEntity = e.id;
			};
		}

		void cleanup() {
			auto & comp = getDebugComponent();
			comp.elements.clear();
		}

	private:
		kengine::DebugGraphicsComponent & getDebugComponent() { return _em.getEntity(_debugEntity).get<kengine::DebugGraphicsComponent>(); }

		void drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color) override {
			auto & comp = getDebugComponent();
			const auto a = toPutils(from);
			const auto b = toPutils(to);
			const auto putilsColor = putils::NormalizedColor{ color[0], color[1], color[2], 1.f };
			comp.elements.emplace_back(kengine::DebugGraphicsComponent::Line{ a }, b, putilsColor, kengine::DebugGraphicsComponent::ReferenceSpace::World);
		}

		void drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color) override {}

		void draw3dText(const btVector3 & location, const char * textString) override {}

		void reportErrorWarning(const char * warningString) override { std::cout << putils::termcolor::red << "[Bullet] " << warningString << putils::termcolor::reset; }

		void setDebugMode(int debugMode) override {}
		int getDebugMode() const override { return DBG_DrawWireframe; }

	private:
		kengine::EntityManager & _em;
		kengine::Entity::ID _debugEntity;
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
	static auto ENABLE_DEBUG = false;
	static auto GRAVITY = 1.f;

	static btDefaultCollisionConfiguration collisionConfiguration;
	static btCollisionDispatcher dispatcher(&collisionConfiguration);
	static btDbvtBroadphase overlappingPairCache;
	static btSequentialImpulseConstraintSolver solver;

	static btDiscreteDynamicsWorld dynamicsWorld(&dispatcher, &overlappingPairCache, &solver, &collisionConfiguration);

	static EntityManager * g_em;

	// declarations
	static void execute(float deltaTime);
	static void onEntityRemoved(Entity & e);
	static putils::vector<Entity::ID, KENGINE_QUERY_POSITION_MAX_RESULTS> queryPosition(const putils::Point3f & pos, float radius);
	//
	EntityCreator * BulletSystem(EntityManager & em) {
		g_em = &em;

		return [](Entity & e) {
			e += functions::Execute{ execute };
			e += functions::OnEntityRemoved{ onEntityRemoved };
			e += functions::QueryPosition{ queryPosition };

			e += AdjustableComponent{
				"Physics", {
					{ "Gravity", &GRAVITY }
#ifndef KENGINE_NDEBUG
					, {"Debug", &ENABLE_DEBUG }
#endif
				}
			};

#ifndef KENGINE_NDEBUG
			debug::drawer = new debug::Drawer(*g_em);
#endif
		};
	}

	// declarations
	static void addBulletComponent(Entity & e, TransformComponent & transform, PhysicsComponent & physics, const Entity & modelEntity);
	static void updateBulletComponent(Entity & e, const TransformComponent & transform, PhysicsComponent & physics, const Entity & modelEntity, bool first = false);
	//
	static void execute(float deltaTime) {
		for (auto & [e, graphics, transform, physics] : g_em->getEntities<GraphicsComponent, TransformComponent, PhysicsComponent>()) {
			if (graphics.model == Entity::INVALID_ID)
				continue;

			const auto & modelEntity = g_em->getEntity(graphics.model);
			if (!modelEntity.has<ModelColliderComponent>())
				continue;
			if (!e.has<BulletPhysicsComponent>())
				addBulletComponent(e, transform, physics, modelEntity);
			else
				updateBulletComponent(e, transform, physics, modelEntity);
		}

		for (auto & [e, bullet, noPhys] : g_em->getEntities<BulletPhysicsComponent, no<PhysicsComponent>>()) {
			onEntityRemoved(e);
			e.detach<BulletPhysicsComponent>();
		}

		dynamicsWorld.setGravity({ 0.f, -GRAVITY, 0.f });
		dynamicsWorld.stepSimulation(deltaTime);

#ifndef KENGINE_NDEBUG
		debug::drawer->cleanup();
		dynamicsWorld.setDebugDrawer(ENABLE_DEBUG ? debug::drawer : nullptr);
		dynamicsWorld.debugDrawWorld();
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

	static void addBulletComponent(Entity & e, TransformComponent & transform, PhysicsComponent & physics, const Entity & modelEntity) {
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
			const auto size = collider.boundingBox.size;

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
					static_assert(putils::magic_enum::enum_count<ModelColliderComponent::Collider::Shape>() == 5);
					break;
				}
			}
			comp.shape->addChildShape(toBullet(transform, collider, skeleton, modelSkeleton, modelComponent), shape);
		}

		btVector3 localInertia{ 0.f, 0.f, 0.f }; {
			if (physics.mass != 0.f)
				comp.shape->calculateLocalInertia(physics.mass, localInertia);
		}

		struct KengineMotionState : public btMotionState {
			KengineMotionState(TransformComponent & transform) : transform(transform) {}

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

			TransformComponent & transform;
		};

		const auto motionState = new KengineMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(physics.mass, motionState, comp.shape, localInertia);
		// btRigidBody::btRigidBodyConstructionInfo rbInfo(physics.mass, motionState, mass != 0.f ? (btCollisionShape*)(new btSphereShape(.5f)) : (btCollisionShape*)(new btBoxShape({ 6.25, 0.125, 6.05 })), localInertia);
		comp.body = new btRigidBody(rbInfo);
		comp.body->setUserIndex((int)e.id);

		dynamicsWorld.addRigidBody(comp.body);
		updateBulletComponent(e, transform, physics, modelEntity, true);
	}

	void detectCollisions(btDynamicsWorld *, btScalar timeStep) {
		const auto numManifolds = dispatcher.getNumManifolds();
		if (numManifolds <= 0)
			return;
		for (const auto & [_, onCollision] : g_em->getEntities<functions::OnCollision>())
			for (int i = 0; i < numManifolds; ++i) {
				const auto contactManifold = dispatcher.getManifoldByIndexInternal(i);
				const auto objectA = (btCollisionObject *)(contactManifold->getBody0());
				const auto objectB = (btCollisionObject *)(contactManifold->getBody1());

				const auto id1 = objectA->getUserIndex();
				const auto id2 = objectB->getUserIndex();
				auto e1 = g_em->getEntity(id1);
				auto e2 = g_em->getEntity(id2);
				onCollision(e1, e2);
			}
	}

	static void updateBulletComponent(Entity & e, const TransformComponent & transform, PhysicsComponent & physics, const Entity & modelEntity, bool first) {
		auto & comp = e.get<BulletPhysicsComponent>();

		const bool kinematic = e.has<KinematicComponent>();

		if (physics.changed || first) {
			// comp.body->clearForces();
			comp.body->setLinearVelocity(toBullet(physics.movement * physics.speed));
			comp.body->setAngularVelocity(btVector3{ physics.pitch, physics.yaw, physics.roll } * physics.speed);

			btVector3 localInertia{ 0.f, 0.f, 0.f };
			if (physics.mass != 0.f)
				comp.body->getCollisionShape()->calculateLocalInertia(physics.mass, localInertia);
			comp.body->setMassProps(physics.mass, localInertia);

			comp.body->forceActivationState(kinematic ? ISLAND_SLEEPING : ACTIVE_TAG);
			comp.body->setWorldTransform(toBullet(transform));
		}
		else if (kinematic) {
			comp.body->setWorldTransform(toBullet(transform));
		}
		else if (!kinematic) {
			physics.movement = toPutils(comp.body->getLinearVelocity()) / physics.speed;
			physics.pitch = comp.body->getAngularVelocity().x();
			physics.yaw = comp.body->getAngularVelocity().y();
			physics.roll = comp.body->getAngularVelocity().z();
		}

		if (kinematic) {
			comp.body->setCollisionFlags(comp.body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			comp.body->setActivationState(WANTS_DEACTIVATION);
		}
		else if (comp.body->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT) { // Kinematic -> not kinematic
			comp.body->setCollisionFlags(comp.body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
			comp.body->setActivationState(ACTIVE_TAG);
		}

		physics.changed = false;

		if (e.has<SkeletonComponent>() && modelEntity.has<ModelSkeletonComponent>()) {
			const auto & skeleton = e.get<SkeletonComponent>();
			const auto & modelSkeleton = modelEntity.get<ModelSkeletonComponent>();
			const auto & modelComponent = modelEntity.get<ModelComponent>();

			int i = 0;
			for (const auto & collider : modelEntity.get<ModelColliderComponent>().colliders) {
				comp.shape->updateChildTransform(i, toBullet(transform, collider, &skeleton, &modelSkeleton, &modelComponent));
				++i;
			}
		}
	}

	static void onEntityRemoved(Entity & e) {
		if (!e.has<BulletPhysicsComponent>())
			return;
		auto & comp = e.get<BulletPhysicsComponent>();
		dynamicsWorld.removeRigidBody(comp.body);
		delete comp.body->getMotionState();
		delete comp.body;
		delete comp.shape;
	}

	static putils::vector<Entity::ID, KENGINE_QUERY_POSITION_MAX_RESULTS> queryPosition(const putils::Point3f & pos, float radius) {
		putils::vector<Entity::ID, KENGINE_QUERY_POSITION_MAX_RESULTS> ret;

		btSphereShape sphere(radius);
		btPairCachingGhostObject ghost;
		btTransform transform;
		transform.setOrigin(toBullet(pos));

		ghost.setCollisionShape(&sphere);
		ghost.setWorldTransform(transform);
		ghost.activate(true);

		struct Callback : btCollisionWorld::ContactResultCallback {
			Callback(btPairCachingGhostObject & ghost, decltype(ret) & results) : ghost(ghost), results(results) {}

			bool needsCollision(btBroadphaseProxy * proxy0) const final {
				return !results.full() && btCollisionWorld::ContactResultCallback::needsCollision(proxy0);
			}

			btScalar addSingleResult(btManifoldPoint & cp, const btCollisionObjectWrapper * colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper * colObj1Wrap, int partId1, int index1) final {
				assert(colObj1Wrap->m_collisionObject == &ghost);
				const auto id = colObj0Wrap->m_collisionObject->getUserIndex();
				if (std::find(results.begin(), results.end(), id) == results.end())
					results.push_back(id);
				return 1.f;
			}

			btPairCachingGhostObject & ghost;
			decltype(ret) & results;
		};

		Callback callback(ghost, ret);
		dynamicsWorld.contactTest(&ghost, callback);

		return ret;
	}
}