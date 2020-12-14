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
#include "data/InstanceComponent.hpp"
#include "data/KinematicComponent.hpp"
#include "data/ModelColliderComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/ModelSkeletonComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/SkeletonComponent.hpp"
#include "data/TransformComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnCollision.hpp"
#include "functions/QueryPosition.hpp"

#include "helpers/matrixHelper.hpp"
#include "helpers/skeletonHelper.hpp"

#include "termcolor.hpp"
#include "magic_enum.hpp"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

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

namespace kengine::bullet {
	struct helpers {
		static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }
		static putils::Point3f toPutils(const btVector3 & vec) { return { vec.getX(), vec.getY(), vec.getZ() }; }
		static btVector3 toBullet(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

		static btTransform toBullet(const TransformComponent & transform) {
			glm::mat4 mat(1.f);

			mat = glm::translate(mat, toVec(transform.boundingBox.position));
			mat = glm::rotate(mat, transform.yaw, { 0.f, 1.f, 0.f });
			mat = glm::rotate(mat, transform.pitch, { 1.f, 0.f, 0.f });
			mat = glm::rotate(mat, transform.roll, { 0.f, 0.f, 1.f });

			btTransform ret;
			ret.setFromOpenGLMatrix(glm::value_ptr(mat));

			return ret;
		}

		static btTransform toBullet(const TransformComponent & parent, const ModelColliderComponent::Collider & collider, const SkeletonComponent * skeleton, const ModelSkeletonComponent * modelSkeleton, const ModelComponent * model) {
			glm::mat4 parentMat(1.f);
			parentMat = glm::scale(parentMat, { -1.f, 1.f, -1.f });

			if (!collider.boneName.empty()) {
				// Apply model scale to bone transformation
				assert(skeleton != nullptr && modelSkeleton != nullptr && model != nullptr);

				const auto worldSpaceBone = skeletonHelper::getBoneMatrix(collider.boneName.c_str(), *skeleton, *modelSkeleton);
				const auto pos = matrixHelper::getPosition(worldSpaceBone);
				parentMat = glm::translate(parentMat, toVec(pos * model->boundingBox.size * parent.boundingBox.size));
				parentMat = glm::translate(parentMat, -toVec(pos));
				parentMat *= worldSpaceBone;
			}

			glm::mat4 colliderMat(1.f);
			colliderMat = glm::translate(colliderMat, toVec(collider.boundingBox.position * parent.boundingBox.size));
			colliderMat = glm::rotate(colliderMat, collider.yaw, { 0.f, 1.f, 0.f });
			colliderMat = glm::rotate(colliderMat, collider.pitch, { 1.f, 0.f, 0.f });
			colliderMat = glm::rotate(colliderMat, collider.roll, { 0.f, 0.f, 1.f });

			const auto mat = parentMat * colliderMat;
			btTransform ret;
			ret.setFromOpenGLMatrix(glm::value_ptr(mat));

			return ret;
		}
	};

#pragma region Debug Drawer
#ifndef KENGINE_NDEBUG
	class Drawer : public btIDebugDraw {
	public:
		Drawer(EntityManager & em) : _em(em) {
			em += [this](Entity & e) {
				e += TransformComponent{};
				e += DebugGraphicsComponent{};
				_debugEntity = e.id;
			};
		}

		void cleanup() {
			auto & comp = getDebugComponent();
			comp.elements.clear();
		}

	private:
		DebugGraphicsComponent & getDebugComponent() { return _em.getEntity(_debugEntity).get<DebugGraphicsComponent>(); }

		void drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color) override {
			auto & comp = getDebugComponent();
			const auto a = helpers::toPutils(from);
			const auto b = helpers::toPutils(to);
			const auto putilsColor = putils::NormalizedColor{ color[0], color[1], color[2], 1.f };
			comp.elements.emplace_back(DebugGraphicsComponent::Line{ a }, b, putilsColor, DebugGraphicsComponent::ReferenceSpace::World);
		}

		void drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color) override {}

		void draw3dText(const btVector3 & location, const char * textString) override {}

		void reportErrorWarning(const char * warningString) override { std::cerr << putils::termcolor::red << "[Bullet] " << warningString << putils::termcolor::reset; }

		void setDebugMode(int debugMode) override {}
		int getDebugMode() const override { return DBG_DrawWireframe; }

	private:
		EntityManager & _em;
		Entity::ID _debugEntity;
	};

	static Drawer * drawer = nullptr;
#endif
#pragma endregion Debug Drawer

	struct impl {
		static inline EntityManager * em;

		static inline struct {
			bool enableDebug = false;
			float gravity = 1.f;
		} adjustables;

		static inline btDefaultCollisionConfiguration collisionConfiguration;
		static inline btCollisionDispatcher dispatcher{ &collisionConfiguration };
		static inline btDbvtBroadphase overlappingPairCache;
		static inline btSequentialImpulseConstraintSolver solver;

		static inline btDiscreteDynamicsWorld dynamicsWorld{ &dispatcher, &overlappingPairCache, &solver, &collisionConfiguration };

		struct BulletPhysicsComponent {
			struct MotionState : public btMotionState {
				void getWorldTransform(btTransform & worldTrans) const final {
					worldTrans = helpers::toBullet(*transform);
				}

				void setWorldTransform(const btTransform & worldTrans) final {
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
			bool active = false;

			~BulletPhysicsComponent() noexcept {
				if (active)
					dynamicsWorld.removeRigidBody(&body);
			}

			BulletPhysicsComponent() noexcept = default;
			BulletPhysicsComponent(BulletPhysicsComponent &&) = default;
			BulletPhysicsComponent & operator=(BulletPhysicsComponent &&) = default;
		};

		static void init(Entity & e) {
			e += functions::Execute{ execute };
			e += functions::QueryPosition{ queryPosition };

			e += AdjustableComponent{
				"Physics", {
					{ "Gravity", &adjustables.gravity }
#ifndef KENGINE_NDEBUG
					, {"Debug", &adjustables.enableDebug }
#endif
				}
			};

#ifndef KENGINE_NDEBUG
			bullet::drawer = new bullet::Drawer(*em);
#endif
		}

		static void execute(float deltaTime) {
			for (auto [e, instance, transform, physics, comp] : em->getEntities<InstanceComponent, TransformComponent, PhysicsComponent, BulletPhysicsComponent>()) {
				const auto model = em->getEntity(instance.model);
				if (!model.has<ModelColliderComponent>())
					continue;
				updateBulletComponent(e, comp, transform, physics, model);
			}

			for (auto [e, instance, transform, physics, noComp] : em->getEntities<InstanceComponent, TransformComponent, PhysicsComponent, no<BulletPhysicsComponent>>()) {
				const auto model = em->getEntity(instance.model);
				if (!model.has<ModelColliderComponent>())
					continue;
				addBulletComponent(e, transform, physics, model);
			}

			for (auto [e, bullet, noPhys] : em->getEntities<BulletPhysicsComponent, no<PhysicsComponent>>())
				e.detach<BulletPhysicsComponent>();

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
			comp.active = true;
			comp.motionState.transform = &transform;

			const auto & modelCollider = modelEntity.get<ModelColliderComponent>();

			const auto skeleton = e.tryGet<SkeletonComponent>();
			const auto modelSkeleton = modelEntity.tryGet<ModelSkeletonComponent>();
			const auto modelComponent = modelEntity.tryGet<ModelComponent>();

			for (const auto & collider : modelCollider.colliders) {
				const auto size = collider.boundingBox.size * transform.boundingBox.size;

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
				comp.shape.addChildShape(helpers::toBullet(transform, collider, skeleton, modelSkeleton, modelComponent), shape);
			}

			btVector3 localInertia{ 0.f, 0.f, 0.f }; {
				if (physics.mass != 0.f)
					comp.shape.calculateLocalInertia(physics.mass, localInertia);
			}

			btRigidBody::btRigidBodyConstructionInfo rbInfo(physics.mass, &comp.motionState, &comp.shape, localInertia);
			comp.body = btRigidBody(rbInfo);
			comp.body.setUserIndex((int)e.id);

			updateBulletComponent(e, comp, transform, physics, modelEntity, true);
			dynamicsWorld.addRigidBody(&comp.body);
		}

		static void updateBulletComponent(Entity & e, BulletPhysicsComponent & comp, const TransformComponent & transform, PhysicsComponent & physics, const Entity & modelEntity, bool first = false) {
			const bool kinematic = e.has<KinematicComponent>();

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

			const auto skeleton = e.tryGet<SkeletonComponent>();
			const auto modelSkeleton = modelEntity.tryGet<ModelSkeletonComponent>();
			if (skeleton && modelSkeleton) {
				const auto & modelComponent = modelEntity.get<ModelComponent>();

				int i = 0;
				for (const auto & collider : modelEntity.get<ModelColliderComponent>().colliders) {
					comp.shape.updateChildTransform(i, helpers::toBullet(transform, collider, skeleton, modelSkeleton, &modelComponent));
					++i;
				}
			}
		}

		static void detectCollisions() {
			const auto numManifolds = dispatcher.getNumManifolds();
			if (numManifolds <= 0)
				return;
			for (const auto & [_, onCollision] : em->getEntities<functions::OnCollision>())
				for (int i = 0; i < numManifolds; ++i) {
					const auto contactManifold = dispatcher.getManifoldByIndexInternal(i);
					const auto objectA = (btCollisionObject *)(contactManifold->getBody0());
					const auto objectB = (btCollisionObject *)(contactManifold->getBody1());

					const auto id1 = objectA->getUserIndex();
					const auto id2 = objectB->getUserIndex();
					auto e1 = em->getEntity(id1);
					auto e2 = em->getEntity(id2);
					onCollision(e1, e2);
				}
		}

		static void queryPosition(const putils::Point3f & pos, float radius, const EntityIteratorFunc & func) {
			btSphereShape sphere(radius);
			btPairCachingGhostObject ghost;
			btTransform transform;
			transform.setOrigin(helpers::toBullet(pos));

			ghost.setCollisionShape(&sphere);
			ghost.setWorldTransform(transform);
			ghost.activate(true);

			struct Callback : btCollisionWorld::ContactResultCallback {
				Callback(btPairCachingGhostObject & ghost, const EntityIteratorFunc & func) : ghost(ghost), func(func) {}

				btScalar addSingleResult(btManifoldPoint & cp, const btCollisionObjectWrapper * colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper * colObj1Wrap, int partId1, int index1) final {
					assert(colObj1Wrap->m_collisionObject == &ghost);
					const auto id = colObj0Wrap->m_collisionObject->getUserIndex();
					func(em->getEntity(id));
					return 1.f;
				}

				btPairCachingGhostObject & ghost;
				const EntityIteratorFunc & func;
			};

			Callback callback(ghost, func);
			dynamicsWorld.contactTest(&ghost, callback);
		}

	};
}

namespace kengine {
	EntityCreator * BulletSystem(EntityManager & em) {
		bullet::impl::em = &em;
		return [](Entity & e) {
			bullet::impl::init(e);
		};
	}

}