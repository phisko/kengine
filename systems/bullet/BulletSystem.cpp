#include "BulletSystem.hpp"

// stl
#include <map>
#include <memory>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

// bullet
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

// magic_enum
#include <magic_enum.hpp>

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/DebugGraphicsComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/KinematicComponent.hpp"
#include "data/ModelColliderComponent.hpp"
#include "data/ModelSkeletonComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/SkeletonComponent.hpp"
#include "data/TransformComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"
#include "functions/OnCollision.hpp"
#include "functions/QueryPosition.hpp"

// kengine helpers
#include "helpers/matrixHelper.hpp"
#include "helpers/skeletonHelper.hpp"
#include "helpers/assertHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace putils {
	inline bool operator<(const Point3f & lhs, const Point3f & rhs) noexcept {
		if (lhs.x < rhs.x) return true;
		if (lhs.x > rhs.x) return false;
		if (lhs.y < rhs.y) return true;
		if (lhs.y > rhs.y) return false;
		if (lhs.z < rhs.z) return true;
		if (lhs.z > rhs.z) return false;
		return false;
	}
}

#include "data/NameComponent.hpp"

namespace kengine {
    static struct {
        bool enableDebug = false;
        bool editorMode = false;
        float gravity = 1.f;
    } adjustables;

    struct BulletSystem {
		static inline btDefaultCollisionConfiguration collisionConfiguration;
		static inline btCollisionDispatcher dispatcher{ &collisionConfiguration };
		static inline btDbvtBroadphase overlappingPairCache;
		static inline btSequentialImpulseConstraintSolver solver;

		static inline btDiscreteDynamicsWorld dynamicsWorld{ &dispatcher, &overlappingPairCache, &solver, &collisionConfiguration };

		struct BulletPhysicsComponent {
			struct MotionState : public btMotionState {
				void getWorldTransform(btTransform & worldTrans) const final {
					KENGINE_PROFILING_SCOPE;
					worldTrans = helpers::toBullet(*transform);
				}

				void setWorldTransform(const btTransform & worldTrans) final {
					KENGINE_PROFILING_SCOPE;

					transform->boundingBox.position = helpers::toPutils(worldTrans.getOrigin());
					glm::mat4 mat;
					worldTrans.getOpenGLMatrix(glm::value_ptr(mat));
					glm::extractEulerAngleYXZ(mat, transform->yaw, transform->pitch, transform->roll);
				}

				TransformComponent * transform;
			};

			btCompoundShape shape{ false };
			btRigidBody body{ { 0.f, nullptr, nullptr } };
			MotionState motionState;

			~BulletPhysicsComponent() noexcept {
				KENGINE_PROFILING_SCOPE;
				dynamicsWorld.removeRigidBody(&body);
			}

			BulletPhysicsComponent() noexcept = default;
			BulletPhysicsComponent(BulletPhysicsComponent &&) noexcept = default;
			BulletPhysicsComponent & operator=(BulletPhysicsComponent &&) noexcept = default;
		};

		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "BulletSystem");

			_r = &r;

			const auto e = r.create();
			r.emplace<functions::Execute>(e, execute);
			r.emplace<functions::QueryPosition>(e, queryPosition);

			r.emplace<AdjustableComponent>(e) = {
				"Physics", {
					{ "Gravity", &adjustables.gravity }
#ifndef KENGINE_NDEBUG
					, {"Debug", &adjustables.enableDebug }
					, {"Editor mode (reload colliders each frame)", &adjustables.editorMode }
#endif
				}
			};

#ifndef KENGINE_NDEBUG
			drawer = new Drawer();
#endif

			r.on_construct<PhysicsComponent>().connect<addOrUpdateBulletComponent>();
			r.on_construct<TransformComponent>().connect<addOrUpdateBulletComponent>();
			r.on_construct<InstanceComponent>().connect<addOrUpdateBulletComponent>();

			r.on_construct<ModelColliderComponent>().connect<updateAllInstances>();
			r.on_update<ModelColliderComponent>().connect<updateAllInstances>();
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Verbose, "Execute", "BulletSystem");

			for (auto [e, bullet] : _r->view<BulletPhysicsComponent>(entt::exclude<PhysicsComponent>).each()) {
				kengine_logf(*_r, Verbose, "Execute/BulletSystem", "Removing BulletComponent from %zu", e);
				_r->remove<BulletPhysicsComponent>(e);
			}

			dynamicsWorld.setGravity({ 0.f, -adjustables.gravity, 0.f });
			dynamicsWorld.stepSimulation(deltaTime);
			detectCollisions();

#ifndef KENGINE_NDEBUG
			drawer->cleanup();
			dynamicsWorld.setDebugDrawer(adjustables.enableDebug ? drawer : nullptr);
			dynamicsWorld.debugDrawWorld();
#endif
		}

		using CollisionShapeMap = std::map<putils::Point3f, std::unique_ptr<btCollisionShape>>;
		template<typename Func>
		static btCollisionShape * getCollisionShape(CollisionShapeMap & shapes, const putils::Vector3f & size, Func && creator) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto it = shapes.find(size);
			if (it != shapes.end())
				return it->second.get();
			const auto ret = creator();
			ret->setLocalScaling({ 1.f, 1.f, 1.f });
			shapes.emplace(size, ret);
			return ret;
		}

		static void addOrUpdateBulletComponent(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!r.all_of<TransformComponent, PhysicsComponent, InstanceComponent>(e))
				return;

			const auto & instance = r.get<InstanceComponent>(e);

			if (!r.all_of<ModelColliderComponent>(instance.model))
				return;

			if (r.all_of<ModelSkeletonComponent>(instance.model) && !r.all_of<SkeletonComponent>(e)) {
				kengine_logf(r, Verbose, "Execute/BulletSystem", "Not adding BulletComponent to %zu because it doesn't have a skeleton yet, while its model does", e);
				return;
			}

			kengine_logf(r, Verbose, "BulletSystem", "Adding BulletComponent to %zu", e);

			auto & transform = r.get<TransformComponent>(e);
			auto & physics = r.get<PhysicsComponent>(e);

			r.remove<BulletPhysicsComponent>(e);
			addBulletComponent(e, transform, physics, instance.model);
		}

		static void updateAllInstances(entt::registry & r, entt::entity modelEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [instanceEntity, instance] : r.view<InstanceComponent>().each())
				if (instance.model == modelEntity)
					addOrUpdateBulletComponent(r, instanceEntity);
		}

		static void addBulletComponent(entt::entity e, TransformComponent & transform, PhysicsComponent & physics, entt::entity modelEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & comp = _r->emplace<BulletPhysicsComponent>(e);
			comp.motionState.transform = &transform;

			const auto name = _r->try_get<NameComponent>(e);
			const auto & modelCollider = _r->get<ModelColliderComponent>(modelEntity);

			const auto skeleton = _r->try_get<SkeletonComponent>(e);
			const auto modelSkeleton = _r->try_get<ModelSkeletonComponent>(modelEntity);
			const auto modelTransform = _r->try_get<TransformComponent>(modelEntity);

			for (const auto & collider : modelCollider.colliders)
				addShape(comp, collider, transform, skeleton, modelSkeleton, modelTransform);

			btVector3 localInertia{ 0.f, 0.f, 0.f }; {
				if (physics.mass != 0.f)
					comp.shape.calculateLocalInertia(physics.mass, localInertia);
			}

			btRigidBody::btRigidBodyConstructionInfo rbInfo(physics.mass, &comp.motionState, &comp.shape, localInertia);
			comp.body = btRigidBody(rbInfo);
			comp.body.setUserIndex(int(e));

			updateBulletComponent(e, comp, transform, physics, modelEntity, true);
			dynamicsWorld.addRigidBody(&comp.body);
		}

		static void addShape(BulletPhysicsComponent & comp, const ModelColliderComponent::Collider & collider, const TransformComponent & transform, const SkeletonComponent * skeleton, const ModelSkeletonComponent * modelSkeleton, const TransformComponent * modelTransform) {
			KENGINE_PROFILING_SCOPE;

			const auto size = collider.transform.boundingBox.size * transform.boundingBox.size;

			btCollisionShape * shape = nullptr; {
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
					static_assert(magic_enum::enum_count<ModelColliderComponent::Collider::Shape>() == 5);
					return;
				}
			}
			comp.shape.addChildShape(helpers::toBullet(transform, collider, skeleton, modelSkeleton, modelTransform), shape);
		}

		static void updateBulletComponent(entt::entity e, BulletPhysicsComponent & comp, const TransformComponent & transform, PhysicsComponent & physics, entt::entity modelEntity, bool first = false) noexcept {
			KENGINE_PROFILING_SCOPE;

			const bool kinematic = _r->all_of<KinematicComponent>(e);

			if (physics.changed || first) {
				// comp.body->clearForces();
				comp.body.setLinearVelocity(helpers::toBullet(physics.movement));
				comp.body.setAngularVelocity(btVector3{ physics.pitch, physics.yaw, physics.roll });

				btVector3 localInertia{ 0.f, 0.f, 0.f };
				if (physics.mass != 0.f)
					comp.body.getCollisionShape()->calculateLocalInertia(physics.mass, localInertia);
				comp.body.setMassProps(physics.mass, localInertia);

				comp.body.forceActivationState(kinematic ? ISLAND_SLEEPING : ACTIVE_TAG);
				comp.body.setWorldTransform(helpers::toBullet(transform));
			}
			else if (kinematic) {
				comp.body.setWorldTransform(helpers::toBullet(transform));
			}
			else if (!kinematic) {
				physics.movement = helpers::toPutils(comp.body.getLinearVelocity());
				physics.pitch = comp.body.getAngularVelocity().x();
				physics.yaw = comp.body.getAngularVelocity().y();
				physics.roll = comp.body.getAngularVelocity().z();
			}

			if (kinematic) {
				comp.body.setCollisionFlags(comp.body.getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
				comp.body.setActivationState(WANTS_DEACTIVATION);
			}
			else if (comp.body.getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT) { // Kinematic -> not kinematic
				comp.body.setCollisionFlags(comp.body.getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
				comp.body.setActivationState(ACTIVE_TAG);
			}

			physics.changed = false;

			const auto skeleton = _r->try_get<SkeletonComponent>(e);
			const auto modelSkeleton = _r->try_get<ModelSkeletonComponent>(modelEntity);

			if (skeleton && modelSkeleton) {
				const auto modelTransform = _r->try_get<TransformComponent>(modelEntity);
				int i = 0;
				for (const auto & collider : _r->get<ModelColliderComponent>(modelEntity).colliders) {
					comp.shape.updateChildTransform(i, helpers::toBullet(transform, collider, skeleton, modelSkeleton, modelTransform));
					++i;
				}
			}
		}

		static void detectCollisions() noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto numManifolds = dispatcher.getNumManifolds();
			if (numManifolds <= 0)
				return;
			kengine_log(*_r, Verbose, "Execute/BulletSystem", "Detecting collisions");
			for (const auto & [callbackEntity, onCollision] : _r->view<functions::OnCollision>().each())
				for (int i = 0; i < numManifolds; ++i) {
					const auto contactManifold = dispatcher.getManifoldByIndexInternal(i);
					const auto objectA = (btCollisionObject *)(contactManifold->getBody0());
					const auto objectB = (btCollisionObject *)(contactManifold->getBody1());

					const auto e1 = entt::entity(objectA->getUserIndex());
					const auto e2 = entt::entity(objectB->getUserIndex());
					kengine_logf(*_r, Verbose, "Execute/BulletSystem/detectCollisions", "Found collision between %zu & %zu", e1, e2);
					onCollision(e1, e2);
				}
		}

		static void queryPosition(const putils::Point3f & pos, float radius, const EntityIteratorFunc & func) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*_r, Verbose, "BulletSystem", "Querying radius %f around position { %f, %f, %f }", radius, pos.x, pos.y, pos.z);

			btSphereShape sphere(radius);
			btPairCachingGhostObject ghost;
			btTransform transform;
			transform.setOrigin(helpers::toBullet(pos));

			ghost.setCollisionShape(&sphere);
			ghost.setWorldTransform(transform);
			ghost.activate(true);

			struct Callback : btCollisionWorld::ContactResultCallback {
				Callback(btPairCachingGhostObject & ghost, const EntityIteratorFunc & func) noexcept : ghost(ghost), func(func) {}

				btScalar addSingleResult(btManifoldPoint & cp, const btCollisionObjectWrapper * colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper * colObj1Wrap, int partId1, int index1) final {
					kengine_assert(*_r, colObj1Wrap->m_collisionObject == &ghost);
					const auto e = entt::entity(colObj0Wrap->m_collisionObject->getUserIndex());
					kengine_logf(*_r, Verbose, "BulletSystem/queryPosition", "Found %zu", e);
					func({ *_r, e });
					return 1.f;
				}

				btPairCachingGhostObject & ghost;
				const EntityIteratorFunc & func;
			};

			Callback callback(ghost, func);
			dynamicsWorld.contactTest(&ghost, callback);
		}

		struct helpers {
			static glm::vec3 toVec(const putils::Point3f & p) noexcept { return { p.x, p.y, p.z }; }
			static putils::Point3f toPutils(const btVector3 & vec) noexcept { return { vec.getX(), vec.getY(), vec.getZ() }; }
			static btVector3 toBullet(const putils::Point3f & p) noexcept { return { p.x, p.y, p.z }; }

			static btTransform toBullet(const TransformComponent & transform) noexcept {
				glm::mat4 mat(1.f);

				mat = glm::translate(mat, toVec(transform.boundingBox.position));
				mat = glm::rotate(mat, transform.yaw, { 0.f, 1.f, 0.f });
				mat = glm::rotate(mat, transform.pitch, { 1.f, 0.f, 0.f });
				mat = glm::rotate(mat, transform.roll, { 0.f, 0.f, 1.f });

				btTransform ret;
				ret.setFromOpenGLMatrix(glm::value_ptr(mat));

				return ret;
			}

			static btTransform toBullet(const TransformComponent & parent, const ModelColliderComponent::Collider & collider, const SkeletonComponent * skeleton, const ModelSkeletonComponent * modelSkeleton, const TransformComponent * modelTransform) noexcept {
				KENGINE_PROFILING_SCOPE;
				glm::mat4 mat{ 1.f };

				if (!collider.boneName.empty()) {
					// Also apply model transform to re-align bones
					mat *= matrixHelper::getModelMatrix({}, modelTransform);
					kengine_assert(*_r, skeleton != nullptr && modelSkeleton != nullptr);
					mat *= skeletonHelper::getBoneMatrix(*_r, collider.boneName.c_str(), *skeleton, *modelSkeleton);
				}

				mat = glm::translate(mat, toVec(collider.transform.boundingBox.position));
				mat = glm::rotate(mat, collider.transform.yaw, { 0.f, 1.f, 0.f });
				mat = glm::rotate(mat, collider.transform.pitch, { 1.f, 0.f, 0.f });
				mat = glm::rotate(mat, collider.transform.roll, { 0.f, 0.f, 1.f });

				btTransform ret;
				ret.setFromOpenGLMatrix(glm::value_ptr(mat));

				return ret;
			}
		};

#ifndef KENGINE_NDEBUG
		struct Drawer : public btIDebugDraw {
		public:
			Drawer() noexcept {
				KENGINE_PROFILING_SCOPE;
				_debugEntity = _r->create();
				_r->emplace<TransformComponent>(_debugEntity);
				_r->emplace<DebugGraphicsComponent>(_debugEntity);
			}

			void cleanup() noexcept {
				KENGINE_PROFILING_SCOPE;
				auto & comp = getDebugComponent();
				comp.elements.clear();
			}

		private:
			DebugGraphicsComponent & getDebugComponent() noexcept { return _r->get<DebugGraphicsComponent>(_debugEntity); }

			void drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color) noexcept override {
				KENGINE_PROFILING_SCOPE;

				DebugGraphicsComponent::Element debugElement; {
					debugElement.type = DebugGraphicsComponent::Type::Line;
					debugElement.line.end = helpers::toPutils(from);
					debugElement.pos = helpers::toPutils(to);
					debugElement.color = putils::NormalizedColor{ color[0], color[1], color[2], 1.f };
					debugElement.referenceSpace = DebugGraphicsComponent::ReferenceSpace::World;
				}
				auto & comp = getDebugComponent();
				comp.elements.emplace_back(std::move(debugElement));
			}

			void drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color) override {}

			void draw3dText(const btVector3 & location, const char * textString) override {}

			void reportErrorWarning(const char * warningString) override { std::cerr << "[Bullet] " << warningString; }

			void setDebugMode(int debugMode) override {}
			int getDebugMode() const override { return DBG_DrawWireframe; }

		private:
			entt::entity _debugEntity;
		};

		static inline Drawer * drawer = nullptr;
#endif

		static inline entt::registry * _r;
	};

	void BulletSystem(entt::registry & r) noexcept {
		BulletSystem::init(r);
	}
}