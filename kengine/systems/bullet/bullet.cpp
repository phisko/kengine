#include "bullet.hpp"

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

// meta
#include "putils/meta/concepts/invocable.hpp"

// putils
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/adjustable.hpp"
#include "kengine/data/debug_graphics.hpp"
#include "kengine/data/instance.hpp"
#include "kengine/data/kinematic.hpp"
#include "kengine/data/model_collider.hpp"
#include "kengine/data/model_skeleton.hpp"
#include "kengine/data/physics.hpp"
#include "kengine/data/skeleton.hpp"
#include "kengine/data/transform.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"
#include "kengine/functions/on_collision.hpp"
#include "kengine/functions/query_position.hpp"

// kengine helpers
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/matrix_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"
#include "kengine/helpers/skeleton_helper.hpp"

namespace putils {
	inline bool operator<(const point3f & lhs, const point3f & rhs) noexcept {
		if (lhs.x < rhs.x)
			return true;
		if (lhs.x > rhs.x)
			return false;
		if (lhs.y < rhs.y)
			return true;
		if (lhs.y > rhs.y)
			return false;
		if (lhs.z < rhs.z)
			return true;
		if (lhs.z > rhs.z)
			return false;
		return false;
	}
}

namespace kengine::systems {
	struct bullet {
		entt::registry & r;

		putils::vector<entt::scoped_connection, 5> connections;

		btDefaultCollisionConfiguration collisionConfiguration;
		btCollisionDispatcher dispatcher{ &collisionConfiguration };
		std::unique_ptr<btDbvtBroadphase> overlappingPairCache = std::make_unique<btDbvtBroadphase>(); // Had to make this a ptr for gcc, otherwise failed to instantiate std::is_constructible
		btSequentialImpulseConstraintSolver solver;

		btDiscreteDynamicsWorld dynamics_world{ &dispatcher, overlappingPairCache.get(), &solver, &collisionConfiguration };

		struct {
			bool enable_debug = false;
			bool editor_mode = false;
			float gravity = 1.f;
		} adjustables;

		struct bullet_data {
			struct motion_state : public btMotionState {
				void getWorldTransform(btTransform & world_trans) const final {
					KENGINE_PROFILING_SCOPE;
					world_trans = system->to_bullet(*transform);
				}

				void setWorldTransform(const btTransform & world_trans) final {
					KENGINE_PROFILING_SCOPE;

					transform->bounding_box.position = system->to_putils(world_trans.getOrigin());
					glm::mat4 mat;
					world_trans.getOpenGLMatrix(glm::value_ptr(mat));
					glm::extractEulerAngleYXZ(mat, transform->yaw, transform->pitch, transform->roll);
				}

				systems::bullet * system;
				data::transform * transform;
			};

			std::unique_ptr<btCompoundShape> shape;
			std::unique_ptr<btRigidBody> body;
			std::unique_ptr<motion_state> state;

			~bullet_data() noexcept {
				KENGINE_PROFILING_SCOPE;
				if (state)
					state->system->dynamics_world.removeRigidBody(body.get());
			}

			bullet_data() noexcept = default;
			bullet_data(bullet_data &&) noexcept = default;
			bullet_data & operator=(bullet_data &&) noexcept = default;
		};

		bullet(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/bullet");

			e.emplace<functions::execute>(putils_forward_to_this(execute));
			e.emplace<functions::query_position>(putils_forward_to_this(query_position));

			e.emplace<data::adjustable>() = {
				"Physics",
				{
					{ "Gravity", &adjustables.gravity },
#ifndef KENGINE_NDEBUG
					{ "Debug", &adjustables.enable_debug },
					{ "Editor mode (reload colliders each frame)", &adjustables.editor_mode },
#endif
				}
			};

			connections.emplace_back(r.on_construct<data::physics>().connect<&bullet::add_or_update_bullet_data>(this));
			connections.emplace_back(r.on_construct<data::transform>().connect<&bullet::add_or_update_bullet_data>(this));
			connections.emplace_back(r.on_construct<data::instance>().connect<&bullet::add_or_update_bullet_data>(this));

			connections.emplace_back(r.on_construct<data::model_collider>().connect<&bullet::update_all_instances>(this));
			connections.emplace_back(r.on_update<data::model_collider>().connect<&bullet::update_all_instances>(this));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "execute", "systems/bullet");

			for (auto [e, bullet] : r.view<bullet_data>(entt::exclude<data::physics>).each()) {
				kengine_logf(r, verbose, "execute/bullet", "Removing BulletComponent from %zu", e);
				r.remove<bullet_data>(e);
			}

			dynamics_world.setGravity({ 0.f, -adjustables.gravity, 0.f });
			dynamics_world.stepSimulation(delta_time);
			detectCollisions();

#ifndef KENGINE_NDEBUG
			drawer.cleanup();
			dynamics_world.setDebugDrawer(adjustables.enable_debug ? &drawer : nullptr);
			dynamics_world.debugDrawWorld();
#endif
		}

		using collision_shape_map = std::map<putils::point3f, std::unique_ptr<btCollisionShape>>;
		template<putils::invocable<btCollisionShape *()> Func>
		btCollisionShape * get_collision_shape(collision_shape_map & shapes, const putils::vec3f & size, Func && creator) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto it = shapes.find(size);
			if (it != shapes.end())
				return it->second.get();
			const auto ret = creator();
			ret->setLocalScaling({ 1.f, 1.f, 1.f });
			shapes.emplace(size, ret);
			return ret;
		}

		void add_or_update_bullet_data(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!r.all_of<data::transform, data::physics, data::instance>(e))
				return;

			const auto & instance = r.get<data::instance>(e);

			if (!r.all_of<data::model_collider>(instance.model))
				return;

			if (r.all_of<data::model_skeleton>(instance.model) && !r.all_of<data::skeleton>(e)) {
				kengine_logf(r, verbose, "execute/bullet", "Not adding BulletComponent to %zu because it doesn't have a skeleton yet, while its model does", e);
				return;
			}

			kengine_logf(r, verbose, "systems/bullet", "Adding BulletComponent to %zu", e);

			auto & transform = r.get<data::transform>(e);
			auto & physics = r.get<data::physics>(e);

			r.remove<bullet_data>(e);
			add_bullet_data(e, transform, physics, instance.model);
		}

		void update_all_instances(entt::registry & r, entt::entity model_entity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [instanceEntity, instance] : r.view<data::instance>().each())
				if (instance.model == model_entity)
					add_or_update_bullet_data(r, instanceEntity);
		}

		void add_bullet_data(entt::entity e, data::transform & transform, data::physics & physics, entt::entity model_entity) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & comp = r.emplace<bullet_data>(e);
			comp.shape = std::make_unique<btCompoundShape>();

			comp.state = std::make_unique<bullet_data::motion_state>();
			comp.state->system = this;
			comp.state->transform = &transform;

			const auto & model_collider = r.get<data::model_collider>(model_entity);

			const auto skeleton = r.try_get<data::skeleton>(e);
			const auto model_skeleton = r.try_get<data::model_skeleton>(model_entity);
			const auto model_transform = r.try_get<data::transform>(model_entity);

			for (const auto & collider : model_collider.colliders)
				add_shape(comp, collider, transform, skeleton, model_skeleton, model_transform);

			btVector3 local_inertia{ 0.f, 0.f, 0.f };
			{
				if (physics.mass != 0.f)
					comp.shape->calculateLocalInertia(physics.mass, local_inertia);
			}

			btRigidBody::btRigidBodyConstructionInfo rigid_body_info(physics.mass, comp.state.get(), comp.shape.get(), local_inertia);
			comp.body = std::make_unique<btRigidBody>(rigid_body_info);
			comp.body->setUserIndex(int(e));

			update_bullet_data(e, comp, transform, physics, model_entity, true);
			dynamics_world.addRigidBody(comp.body.get());
		}

		void add_shape(bullet_data & comp, const data::model_collider::collider & collider, const data::transform & transform, const data::skeleton * skeleton, const data::model_skeleton * model_skeleton, const data::transform * model_transform) {
			KENGINE_PROFILING_SCOPE;

			const auto size = collider.transform.bounding_box.size * transform.bounding_box.size;

			btCollisionShape * shape = nullptr;
			{
				switch (collider.shape) {
					case data::model_collider::collider::box: {
						static collision_shape_map shapes;
						shape = get_collision_shape(shapes, size, [&] { return new btBoxShape({ size.x / 2.f, size.y / 2.f, size.z / 2.f }); });
						break;
					}
					case data::model_collider::collider::capsule: {
						static collision_shape_map shapes;
						shape = get_collision_shape(shapes, size, [&] { return new btCapsuleShape(std::min(size.x, size.z) / 2.f, size.y); });
						break;
					}
					case data::model_collider::collider::cone: {
						static collision_shape_map shapes;
						shape = get_collision_shape(shapes, size, [&] { return new btConeShape(std::min(size.x, size.z) / 2.f, size.y); });
						break;
					}
					case data::model_collider::collider::cylinder: {
						static collision_shape_map shapes;
						shape = get_collision_shape(shapes, size, [&] { return new btCylinderShape({ size.x / 2.f, size.y / 2.f, size.z / 2.f }); });
						break;
					}
					case data::model_collider::collider::sphere: {
						static collision_shape_map shapes;
						shape = get_collision_shape(shapes, size, [&] { return new btSphereShape(std::min(std::min(size.x, size.y), size.y) / 2.f); });
						break;
					}
					default:
						static_assert(magic_enum::enum_count<data::model_collider::collider::shape_type>() == 5);
						return;
				}
			}
			comp.shape->addChildShape(to_bullet(transform, collider, skeleton, model_skeleton, model_transform), shape);
		}

		void update_bullet_data(entt::entity e, bullet_data & comp, const data::transform & transform, data::physics & physics, entt::entity model_entity, bool first = false) noexcept {
			KENGINE_PROFILING_SCOPE;

			const bool kinematic = r.all_of<data::kinematic>(e);

			if (physics.changed || first) {
				// comp.body->clearForces();
				comp.body->setLinearVelocity(to_bullet(physics.movement));
				comp.body->setAngularVelocity(btVector3{ physics.pitch, physics.yaw, physics.roll });

				btVector3 local_inertia{ 0.f, 0.f, 0.f };
				if (physics.mass != 0.f)
					comp.body->getCollisionShape()->calculateLocalInertia(physics.mass, local_inertia);
				comp.body->setMassProps(physics.mass, local_inertia);

				comp.body->forceActivationState(kinematic ? ISLAND_SLEEPING : ACTIVE_TAG);
				comp.body->setWorldTransform(to_bullet(transform));
			}
			else if (kinematic) {
				comp.body->setWorldTransform(to_bullet(transform));
			}
			else if (!kinematic) {
				physics.movement = to_putils(comp.body->getLinearVelocity());
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

			const auto skeleton = r.try_get<data::skeleton>(e);
			const auto model_skeleton = r.try_get<data::model_skeleton>(model_entity);

			if (skeleton && model_skeleton) {
				const auto model_transform = r.try_get<data::transform>(model_entity);
				int i = 0;
				for (const auto & collider : r.get<data::model_collider>(model_entity).colliders) {
					comp.shape->updateChildTransform(i, to_bullet(transform, collider, skeleton, model_skeleton, model_transform));
					++i;
				}
			}
		}

		void detectCollisions() noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto num_manifolds = dispatcher.getNumManifolds();
			if (num_manifolds <= 0)
				return;
			kengine_log(r, verbose, "execute/bullet", "Detecting collisions");
			for (const auto & [callback_entity, on_collision] : r.view<functions::on_collision>().each())
				for (int i = 0; i < num_manifolds; ++i) {
					const auto contact_manifold = dispatcher.getManifoldByIndexInternal(i);
					const auto object_a = (btCollisionObject *)(contact_manifold->getBody0());
					const auto object_b = (btCollisionObject *)(contact_manifold->getBody1());

					const auto e1 = entt::entity(object_a->getUserIndex());
					const auto e2 = entt::entity(object_b->getUserIndex());
					kengine_logf(r, verbose, "execute/bullet/detectCollisions", "Found collision between %zu & %zu", e1, e2);
					on_collision(e1, e2);
				}
		}

		void query_position(const putils::point3f & pos, float radius, const entity_iterator_func & func) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, "systems/bullet", "Querying radius %f around position { %f, %f, %f }", radius, pos.x, pos.y, pos.z);

			btSphereShape sphere(radius);
			btPairCachingGhostObject ghost;
			btTransform transform;
			transform.setOrigin(to_bullet(pos));

			ghost.setCollisionShape(&sphere);
			ghost.setWorldTransform(transform);
			ghost.activate(true);

			struct callback : btCollisionWorld::ContactResultCallback {
				callback(entt::registry & r, btPairCachingGhostObject & ghost, const entity_iterator_func & func) noexcept
					: r(r), ghost(ghost), func(func) {}

				btScalar addSingleResult(btManifoldPoint & cp, const btCollisionObjectWrapper * colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper * colObj1Wrap, int partId1, int index1) final {
					kengine_assert(r, colObj1Wrap->m_collisionObject == &ghost);
					const auto e = entt::entity(colObj0Wrap->m_collisionObject->getUserIndex());
					kengine_logf(r, verbose, "systems/bullet/query_position", "Found %zu", e);
					func({ r, e });
					return 1.f;
				}

				entt::registry & r;
				btPairCachingGhostObject & ghost;
				const entity_iterator_func & func;
			};

			callback callback(r, ghost, func);
			dynamics_world.contactTest(&ghost, callback);
		}

		glm::vec3 to_vec(const putils::point3f & p) noexcept { return { p.x, p.y, p.z }; }
		putils::point3f to_putils(const btVector3 & vec) noexcept { return { vec.getX(), vec.getY(), vec.getZ() }; }
		btVector3 to_bullet(const putils::point3f & p) noexcept { return { p.x, p.y, p.z }; }

		btTransform to_bullet(const data::transform & transform) noexcept {
			glm::mat4 mat(1.f);

			mat = glm::translate(mat, to_vec(transform.bounding_box.position));
			mat = glm::rotate(mat, transform.yaw, { 0.f, 1.f, 0.f });
			mat = glm::rotate(mat, transform.pitch, { 1.f, 0.f, 0.f });
			mat = glm::rotate(mat, transform.roll, { 0.f, 0.f, 1.f });

			btTransform ret;
			ret.setFromOpenGLMatrix(glm::value_ptr(mat));

			return ret;
		}

		btTransform to_bullet(const data::transform & parent, const data::model_collider::collider & collider, const data::skeleton * skeleton, const data::model_skeleton * model_skeleton, const data::transform * model_transform) noexcept {
			KENGINE_PROFILING_SCOPE;
			glm::mat4 mat{ 1.f };

			if (!collider.bone_name.empty()) {
				// Also apply model transform to re-align bones
				mat *= matrix_helper::get_model_matrix({}, model_transform);
				kengine_assert(*g_r, skeleton != nullptr && model_skeleton != nullptr);
				mat *= skeleton_helper::get_bone_matrix(r, collider.bone_name.c_str(), *skeleton, *model_skeleton);
			}

			mat = glm::translate(mat, to_vec(collider.transform.bounding_box.position));
			mat = glm::rotate(mat, collider.transform.yaw, { 0.f, 1.f, 0.f });
			mat = glm::rotate(mat, collider.transform.pitch, { 1.f, 0.f, 0.f });
			mat = glm::rotate(mat, collider.transform.roll, { 0.f, 0.f, 1.f });

			btTransform ret;
			ret.setFromOpenGLMatrix(glm::value_ptr(mat));

			return ret;
		}

#ifndef KENGINE_NDEBUG
		struct drawer : public btIDebugDraw {
		public:
			drawer(systems::bullet & system) noexcept
				: system(system) {
				KENGINE_PROFILING_SCOPE;
				debug_entity = system.r.create();
				system.r.emplace<data::transform>(debug_entity);
				system.r.emplace<data::debug_graphics>(debug_entity);
			}

			void cleanup() noexcept {
				KENGINE_PROFILING_SCOPE;
				auto & comp = get_debug_component();
				comp.elements.clear();
			}

		private:
			data::debug_graphics & get_debug_component() noexcept { return system.r.get<data::debug_graphics>(debug_entity); }

			void drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color) noexcept override {
				KENGINE_PROFILING_SCOPE;

				data::debug_graphics::element debug_element;
				{
					debug_element.type = data::debug_graphics::element_type::line;
					debug_element.line.end = system.to_putils(from);
					debug_element.pos = system.to_putils(to);
					debug_element.color = putils::normalized_color{ color[0], color[1], color[2], 1.f };
					debug_element.relative_to = data::debug_graphics::reference_space::world;
				}
				auto & comp = get_debug_component();
				comp.elements.emplace_back(std::move(debug_element));
			}

			void drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color) override {}

			void draw3dText(const btVector3 & location, const char * text_string) override {}

			void reportErrorWarning(const char * warning_string) override { std::cerr << "[Bullet] " << warning_string; }

			void setDebugMode(int debugMode) override {}
			int getDebugMode() const override { return DBG_DrawWireframe; }

		private:
			systems::bullet & system;
			entt::entity debug_entity;
		};

		drawer drawer{ *this };
#endif
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		bullet,
		bullet::bullet_data
	)
}