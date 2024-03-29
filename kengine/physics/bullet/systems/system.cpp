#include "system.hpp"

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

// kengine
#include "kengine/config/data/configurable.hpp"
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/transform.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/glm/helpers/get_model_matrix.hpp"
#include "kengine/model/data/instance.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/physics/data/model_collider.hpp"
#include "kengine/physics/data/inertia.hpp"
#include "kengine/physics/functions/on_collision.hpp"
#include "kengine/physics/functions/query_position.hpp"
#include "kengine/physics/kinematic/data/kinematic.hpp"
#include "kengine/render/data/debug_graphics.hpp"
#include "kengine/skeleton/data/bone_names.hpp"
#include "kengine/skeleton/data/bone_matrices.hpp"
#include "kengine/skeleton/helpers/get_bone_matrix.hpp"

#include "config.hpp"

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

namespace kengine::physics::bullet {
	static constexpr auto log_category = "bullet";

	struct system {
		entt::registry & r;
		const config * cfg = nullptr;

		struct processed {};
		kengine::new_entity_processor<processed, core::transform, inertia, model::instance> processor{ r, putils_forward_to_this(add_or_update_bullet_data) };

		const entt::scoped_connection connections[6] = {
			r.on_construct<model_collider>().connect<&system::update_all_instances>(this),
			r.on_update<model_collider>().connect<&system::update_all_instances>(this),
			r.on_construct<skeleton::bone_names>().connect<&system::update_all_instances>(this),
			r.on_update<skeleton::bone_names>().connect<&system::update_all_instances>(this),
			r.on_construct<skeleton::bone_matrices>().connect<&system::on_skeleton_updated>(this),
			r.on_update<skeleton::bone_matrices>().connect<&system::on_skeleton_updated>(this),
		};

		btDefaultCollisionConfiguration collisionConfiguration;
		btCollisionDispatcher dispatcher{ &collisionConfiguration };
		std::unique_ptr<btDbvtBroadphase> overlappingPairCache = std::make_unique<btDbvtBroadphase>(); // Had to make this a ptr for gcc, otherwise failed to instantiate std::is_constructible
		btSequentialImpulseConstraintSolver solver;

		btDiscreteDynamicsWorld dynamics_world{ &dispatcher, overlappingPairCache.get(), &solver, &collisionConfiguration };

		struct bullet_data {
			struct motion_state : public btMotionState {
				void getWorldTransform(btTransform & world_trans) const final {
					KENGINE_PROFILING_SCOPE;
					world_trans = owning_system->to_bullet(*transform);
				}

				void setWorldTransform(const btTransform & world_trans) final {
					KENGINE_PROFILING_SCOPE;

					transform->bounding_box.position = owning_system->to_putils(world_trans.getOrigin());
					::glm::mat4 mat;
					world_trans.getOpenGLMatrix(::glm::value_ptr(mat));
					::glm::extractEulerAngleYXZ(mat, transform->yaw, transform->pitch, transform->roll);
				}

				system * owning_system;
				core::transform * transform;
			};

			std::unique_ptr<btCompoundShape> shape;
			std::unique_ptr<btRigidBody> body;
			std::unique_ptr<motion_state> state;

			~bullet_data() noexcept {
				KENGINE_PROFILING_SCOPE;
				if (state)
					state->owning_system->dynamics_world.removeRigidBody(body.get());
			}

			bullet_data() noexcept = default;
			bullet_data(bullet_data &&) noexcept = default;
			bullet_data & operator=(bullet_data &&) noexcept = default;
		};

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));
			e.emplace<kengine::physics::query_position>(putils_forward_to_this(query_position));

			e.emplace<core::name>("Physics");
			e.emplace<kengine::config::configurable>();
			cfg = &e.emplace<config>();

			processor.process();
		}

		~system() noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine_log(r, log, log_category, "Shutting down");
			// Make sure these are destroyed before the system
			r.storage<bullet_data>().clear();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			processor.process();

			kengine_log(r, very_verbose, log_category, "Removing obsolete bullet_data");
			for (auto [e, bullet] : r.view<bullet_data>(entt::exclude<inertia>).each()) {
				kengine_logf(r, verbose, log_category, "Removing bullet_data from {}", e);
				r.remove<bullet_data>(e);
			}

			kengine_log(r, very_verbose, log_category, "Updating gravity");
			dynamics_world.setGravity({ 0.f, -cfg->gravity, 0.f });

			kengine_log(r, very_verbose, log_category, "Stepping simulation");
			dynamics_world.stepSimulation(delta_time);
			detect_collisions();

#ifndef KENGINE_NDEBUG
			drawer.cleanup();
			dynamics_world.setDebugDrawer(cfg->enable_debug ? &drawer : nullptr);
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

		void on_skeleton_updated(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (r.all_of<core::transform, inertia, model::instance>(e))
				add_or_update_bullet_data(e, r.get<core::transform>(e), r.get<inertia>(e), r.get<model::instance>(e));
		}

		void add_or_update_bullet_data(entt::entity e, core::transform & transform, inertia & inertia, const model::instance & instance) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!r.all_of<model_collider>(instance.model)) {
				kengine_logf(r, verbose, log_category, "Not adding bullet_data to {} because its model doesn't have a model_collider", e);
				return;
			}

			if (r.all_of<skeleton::bone_names>(instance.model) && !r.all_of<skeleton::bone_matrices>(e)) {
				kengine_logf(r, verbose, log_category, "Not adding bullet_data to {} because it doesn't have a skeleton yet, while its model does", e);
				return;
			}

			kengine_logf(r, verbose, log_category, "Adding bullet_data to {}", e);

			r.remove<bullet_data>(e);
			add_bullet_data(e, transform, inertia, instance.model);
		}

		void update_all_instances(entt::registry & r, entt::entity model_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Updating all instances of {}", model_entity);

			for (const auto & [e, transform, inertia, instance] : r.view<core::transform, inertia, model::instance>().each())
				if (instance.model == model_entity)
					add_or_update_bullet_data(e, transform, inertia, instance);
		}

		void add_bullet_data(entt::entity e, core::transform & transform, inertia & inertia, entt::entity model_entity) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & comp = r.emplace<bullet_data>(e);
			comp.shape = std::make_unique<btCompoundShape>();

			comp.state = std::make_unique<bullet_data::motion_state>();
			comp.state->owning_system = this;
			comp.state->transform = &transform;

			const auto & model_collider = r.get<kengine::physics::model_collider>(model_entity);

			const auto skeleton = r.try_get<skeleton::bone_matrices>(e);
			const auto bone_names = r.try_get<skeleton::bone_names>(model_entity);
			const auto model_transform = r.try_get<core::transform>(model_entity);

			for (const auto & collider : model_collider.colliders)
				add_shape(comp, collider, transform, skeleton, bone_names, model_transform);

			btVector3 local_inertia{ 0.f, 0.f, 0.f };
			{
				if (inertia.mass != 0.f)
					comp.shape->calculateLocalInertia(inertia.mass, local_inertia);
			}

			btRigidBody::btRigidBodyConstructionInfo rigid_body_info(inertia.mass, comp.state.get(), comp.shape.get(), local_inertia);
			comp.body = std::make_unique<btRigidBody>(rigid_body_info);
			comp.body->setUserIndex(int(e));

			update_bullet_data(e, comp, transform, inertia, model_entity, true);
			dynamics_world.addRigidBody(comp.body.get());
		}

		void add_shape(bullet_data & comp, const model_collider::collider & collider, const core::transform & transform, const skeleton::bone_matrices * skeleton, const skeleton::bone_names * bone_names, const core::transform * model_transform) {
			KENGINE_PROFILING_SCOPE;

			const auto size = collider.transform.bounding_box.size * transform.bounding_box.size;

			btCollisionShape * shape = nullptr;
			{
				switch (collider.shape) {
					case model_collider::collider::box: {
						static collision_shape_map shapes;
						shape = get_collision_shape(shapes, size, [&] { return new btBoxShape({ size.x / 2.f, size.y / 2.f, size.z / 2.f }); });
						break;
					}
					case model_collider::collider::capsule: {
						static collision_shape_map shapes;
						shape = get_collision_shape(shapes, size, [&] { return new btCapsuleShape(std::min(size.x, size.z) / 2.f, size.y); });
						break;
					}
					case model_collider::collider::cone: {
						static collision_shape_map shapes;
						shape = get_collision_shape(shapes, size, [&] { return new btConeShape(std::min(size.x, size.z) / 2.f, size.y); });
						break;
					}
					case model_collider::collider::cylinder: {
						static collision_shape_map shapes;
						shape = get_collision_shape(shapes, size, [&] { return new btCylinderShape({ size.x / 2.f, size.y / 2.f, size.z / 2.f }); });
						break;
					}
					case model_collider::collider::sphere: {
						static collision_shape_map shapes;
						shape = get_collision_shape(shapes, size, [&] { return new btSphereShape(std::min(std::min(size.x, size.y), size.y) / 2.f); });
						break;
					}
					default:
						static_assert(magic_enum::enum_count<model_collider::collider::shape_type>() == 5);
						return;
				}
			}
			comp.shape->addChildShape(to_bullet(transform, collider, skeleton, bone_names, model_transform), shape);
		}

		void update_bullet_data(entt::entity e, bullet_data & comp, const core::transform & transform, inertia & inertia, entt::entity model_entity, bool first = false) noexcept {
			KENGINE_PROFILING_SCOPE;

			const bool kinematic = r.all_of<kinematic::kinematic>(e);

			if (inertia.changed || first) {
				// comp.body->clearForces();
				comp.body->setLinearVelocity(to_bullet(inertia.movement));
				comp.body->setAngularVelocity(btVector3{ inertia.pitch, inertia.yaw, inertia.roll });

				btVector3 local_inertia{ 0.f, 0.f, 0.f };
				if (inertia.mass != 0.f)
					comp.body->getCollisionShape()->calculateLocalInertia(inertia.mass, local_inertia);
				comp.body->setMassProps(inertia.mass, local_inertia);

				comp.body->forceActivationState(kinematic ? ISLAND_SLEEPING : ACTIVE_TAG);
				comp.body->setWorldTransform(to_bullet(transform));
			}
			else if (kinematic) {
				comp.body->setWorldTransform(to_bullet(transform));
			}
			else if (!kinematic) {
				inertia.movement = to_putils(comp.body->getLinearVelocity());
				inertia.pitch = comp.body->getAngularVelocity().x();
				inertia.yaw = comp.body->getAngularVelocity().y();
				inertia.roll = comp.body->getAngularVelocity().z();
			}

			if (kinematic) {
				comp.body->setCollisionFlags(comp.body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
				comp.body->setActivationState(WANTS_DEACTIVATION);
			}
			else if (comp.body->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT) { // Kinematic -> not kinematic
				comp.body->setCollisionFlags(comp.body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
				comp.body->setActivationState(ACTIVE_TAG);
			}

			inertia.changed = false;

			const auto skeleton = r.try_get<skeleton::bone_matrices>(e);
			const auto bone_names = r.try_get<skeleton::bone_names>(model_entity);

			if (skeleton && bone_names) {
				const auto model_transform = r.try_get<core::transform>(model_entity);
				int i = 0;
				for (const auto & collider : r.get<model_collider>(model_entity).colliders) {
					comp.shape->updateChildTransform(i, to_bullet(transform, collider, skeleton, bone_names, model_transform));
					++i;
				}
			}
		}

		void detect_collisions() noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto num_manifolds = dispatcher.getNumManifolds();
			if (num_manifolds <= 0)
				return;
			kengine_log(r, very_verbose, log_category, "Detecting collisions");
			for (const auto & [callback_entity, on_collision] : r.view<kengine::physics::on_collision>().each())
				for (int i = 0; i < num_manifolds; ++i) {
					const auto contact_manifold = dispatcher.getManifoldByIndexInternal(i);
					const auto object_a = (btCollisionObject *)(contact_manifold->getBody0());
					const auto object_b = (btCollisionObject *)(contact_manifold->getBody1());

					const auto e1 = entt::entity(object_a->getUserIndex());
					const auto e2 = entt::entity(object_b->getUserIndex());
					kengine_logf(r, verbose, log_category, "Collision between {} & {}", e1, e2);
					on_collision(e1, e2);
				}
		}

		void query_position(const putils::point3f & pos, float radius, const entity_iterator_func & func) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Querying radius {} around position {}", radius, pos);

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
					kengine_logf(r, verbose, log_category, "Found {}", e);
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

		::glm::vec3 to_vec(const putils::point3f & p) noexcept { return { p.x, p.y, p.z }; }
		putils::point3f to_putils(const btVector3 & vec) noexcept { return { vec.getX(), vec.getY(), vec.getZ() }; }
		btVector3 to_bullet(const putils::point3f & p) noexcept { return { p.x, p.y, p.z }; }

		btTransform to_bullet(const core::transform & transform) noexcept {
			kengine_log(r, very_verbose, log_category, "Converting transform for entity");

			::glm::mat4 mat(1.f);

			mat = ::glm::translate(mat, to_vec(transform.bounding_box.position));
			mat = ::glm::rotate(mat, transform.yaw, { 0.f, 1.f, 0.f });
			mat = ::glm::rotate(mat, transform.pitch, { 1.f, 0.f, 0.f });
			mat = ::glm::rotate(mat, transform.roll, { 0.f, 0.f, 1.f });

			btTransform ret;
			ret.setFromOpenGLMatrix(::glm::value_ptr(mat));

			return ret;
		}

		btTransform to_bullet(const core::transform & parent, const model_collider::collider & collider, const skeleton::bone_matrices * skeleton, const skeleton::bone_names * bone_names, const core::transform * model_transform) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Converting transform for collider");

			::glm::mat4 mat{ 1.f };

			if (!collider.bone_name.empty() && skeleton && bone_names) {
				// Also apply model transform to re-align bones
				mat *= glm::get_model_matrix({}, model_transform);
				mat *= skeleton::get_bone_matrix(r, collider.bone_name.c_str(), *skeleton, *bone_names);
			}

			mat = ::glm::translate(mat, to_vec(collider.transform.bounding_box.position));
			mat = ::glm::rotate(mat, collider.transform.yaw, { 0.f, 1.f, 0.f });
			mat = ::glm::rotate(mat, collider.transform.pitch, { 1.f, 0.f, 0.f });
			mat = ::glm::rotate(mat, collider.transform.roll, { 0.f, 0.f, 1.f });

			btTransform ret;
			ret.setFromOpenGLMatrix(::glm::value_ptr(mat));

			return ret;
		}

#ifndef KENGINE_NDEBUG
		struct drawer : public btIDebugDraw {
		public:
			drawer(system & system) noexcept
				: owning_system(system) {
				KENGINE_PROFILING_SCOPE;
				kengine_log(owning_system.r, verbose, log_category, "Constructing debug drawer");
				debug_entity = owning_system.r.create();
				owning_system.r.emplace<core::transform>(debug_entity);
				owning_system.r.emplace<render::debug_graphics>(debug_entity);
			}

			void cleanup() noexcept {
				KENGINE_PROFILING_SCOPE;
				kengine_log(owning_system.r, very_verbose, log_category, "Cleaning up debug drawer");
				auto & comp = get_debug_component();
				comp.elements.clear();
			}

		private:
			render::debug_graphics & get_debug_component() noexcept { return owning_system.r.get<render::debug_graphics>(debug_entity); }

			void drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color) noexcept override {
				KENGINE_PROFILING_SCOPE;

				render::debug_graphics::element debug_element;
				{
					debug_element.type = render::debug_graphics::element_type::line;
					debug_element.line.end = owning_system.to_putils(from);
					debug_element.pos = owning_system.to_putils(to);
					debug_element.color = putils::normalized_color{ color[0], color[1], color[2], 1.f };
					debug_element.relative_to = render::debug_graphics::reference_space::world;
				}
				auto & comp = get_debug_component();
				comp.elements.emplace_back(std::move(debug_element));
			}

			void drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color) override {}

			void draw3dText(const btVector3 & location, const char * text_string) override {}

			void reportErrorWarning(const char * warning_string) override {
				kengine_log(owning_system.r, warning, log_category, warning_string);
			}

			void setDebugMode(int debugMode) override {}
			int getDebugMode() const override { return DBG_DrawWireframe; }

		private:
			system & owning_system;
			entt::entity debug_entity;
		};

		drawer drawer{ *this };
#endif
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::bullet_data,
		system::processed
	)
}