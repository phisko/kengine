#include "system.hpp"

// stl
#include <algorithm>
#include <atomic>
#include <execution>
#include <future>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gl
#include <GL/glew.h>

// imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// putils
#include "putils/forward_to.hpp"
#include "putils/range.hpp"
#include "putils/thread_name.hpp"

// kreogl
#include "kreogl/animation/animated_object.hpp"
#include "kreogl/impl/texture/texture_data.hpp"
#include "kreogl/loaders/assimp/assimp.hpp"
#include "kreogl/window.hpp"
#include "kreogl/world.hpp"

// kengine
#include "kengine/adjustable/data/values.hpp"
#include "kengine/async/helpers/process_results.hpp"
#include "kengine/async/helpers/start_task.hpp"
#include "kengine/core/data/transform.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/glm/helpers/get_model_matrix.hpp"
#include "kengine/glm/helpers/glm_formatter.hpp"
#include "kengine/imgui/data/context.hpp"
#include "kengine/imgui/data/scale.hpp"
#include "kengine/imgui/helpers/get_scale.hpp"
#include "kengine/instance/data/instance.hpp"
#include "kengine/instance/helpers/get_model.hpp"
#include "kengine/instance/helpers/try_get_model.hpp"
#include "kengine/main_loop/data/keep_alive.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/render/animation/data/animation.hpp"
#include "kengine/render/animation/data/files.hpp"
#include "kengine/render/animation/data/model_animation.hpp"
#include "kengine/render/data/asset.hpp"
#include "kengine/render/data/camera.hpp"
#include "kengine/render/data/debug_graphics.hpp"
#include "kengine/render/data/drawable.hpp"
#include "kengine/render/data/god_rays.hpp"
#include "kengine/render/data/light.hpp"
#include "kengine/render/data/model_data.hpp"
#include "kengine/render/data/no_shadow.hpp"
#include "kengine/render/data/sky_box.hpp"
#include "kengine/render/data/sprite.hpp"
#include "kengine/render/data/text.hpp"
#include "kengine/render/data/viewport.hpp"
#include "kengine/render/data/window.hpp"
#include "kengine/render/functions/get_entity_in_pixel.hpp"
#include "kengine/render/functions/get_position_in_pixel.hpp"
#include "kengine/render/glfw/data/window.hpp"
#include "kengine/render/helpers/convert_to_screen_percentage.hpp"
#include "kengine/render/helpers/entity_appears_in_viewport.hpp"
#include "kengine/render/helpers/get_facings.hpp"
#include "kengine/render/helpers/get_viewport_for_pixel.hpp"
#include "kengine/render/kreogl/data/animation_files.hpp"
#include "kengine/render/kreogl/data/debug_graphics.hpp"
#include "kengine/render/kreogl/data/model.hpp"
#include "kengine/render/kreogl/helpers/putils_to_glm.hpp"
#include "kengine/render/kreogl/shaders/highlight_shader.hpp"
#include "kengine/skeleton/data/bone_names.hpp"
#include "kengine/skeleton/data/skeleton.hpp"

namespace kengine::render::kreogl {
	static constexpr auto log_category = "render_kreogl";

	struct system {
		entt::registry & r;

		struct processed_window {};
		kengine::new_entity_processor<processed_window, render::window> window_processor{ r, putils_forward_to_this(create_window) };

		struct processed_model {};
		kengine::new_entity_processor<processed_model, render::asset> model_processor{ r, putils_forward_to_this(create_model_from_disk) };

		struct processed_animation_files {};
		kengine::new_entity_processor<processed_animation_files, render::animation::files> animation_files_processor{ r, putils_forward_to_this(load_animation_files) };

		struct processed_sky_box {};
		kengine::new_entity_processor<processed_sky_box, render::sky_box_model> sky_box_processor{ r, putils_forward_to_this(create_sky_box_from_disk) };

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			auto & scale = e.emplace<imgui::scale>();
			e.emplace<adjustable::values>() = {
				"ImGui",
				{
					{ "Scale", &scale.modifier },
				}
			};

			e.emplace<render::get_entity_in_pixel>(putils_forward_to_this(get_entity_in_pixel));
			e.emplace<render::get_position_in_pixel>(putils_forward_to_this(get_position_in_pixel));

			window_processor.process();
			model_processor.process();
			animation_files_processor.process();
			sky_box_processor.process();
		}

		void create_window(entt::entity window_entity, render::window & window) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, log, log_category, "Initializing window for {}", window_entity);

			// Ask the GLFW system to initialize the window
			window.assigned_system = "Kreogl";
			r.emplace<glfw::window_init>(window_entity) = {
				.set_hints = [this]() noexcept {
					kengine_log(r, verbose, log_category, "Setting window hints");
					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
					glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef KENGINE_NDEBUG
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
				},
				.on_window_created = [this, window_entity]() noexcept {
					kengine_log(r, verbose, log_category, "GLFW window created");
					kengine_log(r, verbose, log_category, "Creating kreogl window");
					const auto & glfw_window = r.get<glfw::window>(window_entity);
					auto & kreogl_window = r.emplace<::kreogl::window>(window_entity, *glfw_window.ptr.get());
					kreogl_window.remove_camera(kreogl_window.get_default_camera());
					init_imgui(window_entity);
					// clang-format off
				},
				// clang-format on
			};
		}

		void init_imgui(entt::entity window_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing ImGui");

			r.emplace<imgui::context>(window_entity, ImGui::CreateContext());

			auto & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigViewportsNoTaskBarIcon = true;

			const auto & glfw_comp = r.get<glfw::window>(window_entity);
			ImGui_ImplGlfw_InitForOpenGL(glfw_comp.ptr.get(), true);
			ImGui_ImplOpenGL3_Init();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			window_processor.process();
			model_processor.process();
			animation_files_processor.process();
			sky_box_processor.process();

			complete_loading_tasks();
			load_models_to_opengl();
			create_missing_objects();
			tick_animations(delta_time);
			draw();
		}

		void create_model_from_disk(entt::entity model_entity, const render::asset & asset) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Creating model from disk for {} ({})", model_entity, asset.file);

			const auto & file = asset.file.c_str();

			if (::kreogl::texture_data::is_supported_format(file))
				kengine::async::start_task(
					r, model_entity,
					async::task::string("kreogl: load {}", file),
					std::async(std::launch::async, [&file] {
						KENGINE_PROFILING_SCOPE;
						const putils::scoped_thread_name thread_name(putils::string<64>("Load {}", file));
						return ::kreogl::texture_data(file);
					})
				);
			else if (::kreogl::assimp::is_supported_file_format(asset.file.c_str()))
				kengine::async::start_task(
					r, model_entity,
					async::task::string("kreogl: load {}", file),
					std::async(std::launch::async, [&file] {
						KENGINE_PROFILING_SCOPE;
						const putils::scoped_thread_name thread_name(putils::string<64>("Load {}", file));
						return ::kreogl::assimp::load_model_data(file);
					})
				);
		}

		struct animation_files_task_result {
			struct animation_file {
				const char * file_name;
				std::unique_ptr<::kreogl::animation_file> kreogl_animation_file;
			};
			entt::entity model_entity;
			std::vector<animation_file> animation_files;
		};

		void load_animation_files(entt::entity model_entity, const animation::files & animation_files) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Loading animation files for {}", model_entity);

			const auto task_entity = r.create(); // A task might be attached to model_entity to load the model itself
			kengine::async::start_task(
				r, task_entity,
				async::task::string("kreogl: load animation files for {}", model_entity),
				std::async(std::launch::async, [&animation_files, model_entity] {
					KENGINE_PROFILING_SCOPE;
					const putils::scoped_thread_name thread_name(putils::string<64>("Load animation files for {}", model_entity));

					animation_files_task_result result;
					result.model_entity = model_entity;
					result.animation_files.resize(animation_files.list.size());

					// Profiling this shows that:
					// * with par_unseq, each thread takes ~700ms
					// * with no execution policy, each call takes ~50ms
					// This isn't a performance loss so I'm leaving it as is, but I haven't found an explanation so far
					std::transform(
						std::execution::par_unseq,
						putils_range(animation_files.list),
						result.animation_files.begin(),
						[](const std::string & file) {
							const putils::scoped_thread_name thread_name(putils::string<128>("Animation loader for {}", file));
							return animation_files_task_result::animation_file{ file.c_str(), ::kreogl::assimp::load_animation_file(file.c_str()) };
						}
					);
					return result;
				})
			);
		}

		void add_animations_to_model_animation_component(animation::model_animation & model_animation, const char * file, const ::kreogl::animation_file & animations) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & animation : animations.animations) {
				std::string name = file;
				name += '/';
				name += animation->name;
				kengine_logf(r, verbose, log_category, "Adding animation {}", name);
				model_animation.animations.push_back(animation::model_animation::anim{
					.name = std::move(name),
					.total_time = animation->total_time,
					.ticks_per_second = animation->ticks_per_second,
				});
			}
		}

		struct sky_box_task_result {
			entt::entity sky_box_entity;
			::kreogl::texture_data left;
			::kreogl::texture_data right;
			::kreogl::texture_data top;
			::kreogl::texture_data bottom;
			::kreogl::texture_data front;
			::kreogl::texture_data back;
		};

		void create_sky_box_from_disk(entt::entity sky_box_entity, const sky_box_model & sky_box) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Loading skybox for {}", sky_box_entity);

			const auto task_entity = r.create();
			kengine::async::start_task(
				r, task_entity,
				async::task::string("kreogl: load sky_box for {}", sky_box_entity),
				std::async(std::launch::async, [sky_box_entity, &sky_box] {
					KENGINE_PROFILING_SCOPE;
					const putils::scoped_thread_name thread_name(putils::string<64>("Load sky_box for {}", sky_box_entity));

					return sky_box_task_result{
						sky_box_entity,
						sky_box.left.c_str(), sky_box.right.c_str(),
						sky_box.top.c_str(), sky_box.bottom.c_str(),
						sky_box.front.c_str(), sky_box.back.c_str()
					};
				})
			);
		}

		void complete_loading_tasks() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Completing loading tasks");

			using namespace std::chrono_literals;

			kengine::async::process_results<::kreogl::texture_data>(r, [this](entt::entity e, ::kreogl::texture_data && texture_data) {
				r.emplace<::kreogl::texture>(e, texture_data.load_to_texture());
			});

			kengine::async::process_results<::kreogl::assimp_model_data>(r, [this](entt::entity e, ::kreogl::assimp_model_data && model_data) {
				// Sync properties from loaded model to kengine components
				const auto & asset = r.get<render::asset>(e);
				add_animations_to_model_animation_component(r.get_or_emplace<animation::model_animation>(e), asset.file.c_str(), *model_data.animations);
				if (model_data.skeleton) {
					auto & bone_names = r.emplace<skeleton::bone_names>(e);
					for (const auto & mesh : model_data.skeleton->meshes)
						bone_names.meshes.push_back(skeleton::bone_names::mesh{
							.bone_names = mesh.bone_names,
						});
				}

				r.emplace<model>(e, ::kreogl::assimp::load_animated_model(std::move(model_data)));
			});

			kengine::async::process_results<animation_files_task_result>(r, [this](entt::entity task_entity, animation_files_task_result && result) {
				auto & model_animation = r.get_or_emplace<animation::model_animation>(result.model_entity);
				auto & kreogl_animation_files = r.emplace<animation_files>(result.model_entity);
				for (auto & animation_file : result.animation_files) {
					add_animations_to_model_animation_component(model_animation, animation_file.file_name, *animation_file.kreogl_animation_file);
					kreogl_animation_files.files.push_back(std::move(animation_file.kreogl_animation_file));
				}
				r.destroy(task_entity);
			});

			kengine::async::process_results<sky_box_task_result>(r, [this](entt::entity task_entity, sky_box_task_result && result) noexcept {
				r.emplace<::kreogl::skybox_texture>(result.sky_box_entity, result.left, result.right, result.top, result.bottom, result.front, result.back);
				r.destroy(task_entity);
			});
		}

		void load_models_to_opengl() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Loading model data to OpenGL");

			for (const auto & [model_entity, model_data] : r.view<model_data>(entt::exclude<model>).each())
				create_model_from_model_data(model_entity, model_data);
		}

		void create_model_from_model_data(entt::entity model_entity, const model_data & model_data) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Creating model from model data for {}", model_entity);

			::kreogl::model_data kreogl_model_data;

			for (const auto & mesh_data : model_data.meshes) {
				static const std::unordered_map<putils::meta::type_index, GLenum> types = {
					{ putils::meta::type<char>::index, GL_BYTE },
					{ putils::meta::type<unsigned char>::index, GL_UNSIGNED_BYTE },
					{ putils::meta::type<short>::index, GL_SHORT },
					{ putils::meta::type<unsigned short>::index, GL_UNSIGNED_SHORT },
					{ putils::meta::type<int>::index, GL_INT },
					{ putils::meta::type<unsigned int>::index, GL_UNSIGNED_INT },
					{ putils::meta::type<float>::index, GL_FLOAT },
					{ putils::meta::type<double>::index, GL_DOUBLE }
				};

				kreogl_model_data.meshes.push_back(::kreogl::mesh_data{
					.vertices = {
						.nb_elements = mesh_data.vertices.nb_elements,
						.element_size = mesh_data.vertices.element_size,
						.data = mesh_data.vertices.data,
					},
					.indices = {
						.nb_elements = mesh_data.indices.nb_elements,
						.element_size = mesh_data.indices.element_size,
						.data = mesh_data.indices.data,
					},
					.index_type = types.at(mesh_data.index_type),
				});
			}

			for (const auto & vertex_attribute : model_data.vertex_attributes)
				kreogl_model_data.vertex_attribute_offsets.push_back(vertex_attribute.offset);

			kreogl_model_data.vertex_size = model_data.vertex_size;

			// We assume custom-built meshes are voxels, might need a better alternative to this
			const auto & vertex_specification = ::kreogl::vertex_specification::position_color;
			r.emplace<model>(model_entity, std::make_unique<::kreogl::animated_model>(vertex_specification, kreogl_model_data));
		}

		void create_missing_objects() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Creating missing objects");

			for (const auto & [entity, instance] : r.view<instance::instance>(entt::exclude<::kreogl::animated_object, ::kreogl::sprite_2d, ::kreogl::sprite_3d, sky_box>).each()) {
				if (instance.model == entt::null) {
					kengine_logf(r, warning, log_category, "{} has no model", entity);
					continue;
				}

				const auto model_entity = instance.model;
				kengine_logf(r, verbose, log_category, "Creating object for {} (model {})", entity, model_entity);

				if (const auto kreogl_model = r.try_get<model>(model_entity)) {
					kengine_logf(r, verbose, log_category, "Creating animated_object to {}", entity);
					r.emplace<::kreogl::animated_object>(entity).model = kreogl_model->ptr.get();
					if (r.all_of<skeleton::bone_names>(model_entity)) {
						kengine_logf(r, verbose, log_category, "Adding skeleton to {}", entity);
						r.emplace<skeleton::skeleton>(entity);
					}
				}

				if (const auto kreogl_texture = r.try_get<::kreogl::texture>(model_entity)) {
					if (r.all_of<sprite_2d>(entity)) {
						kengine_logf(r, verbose, log_category, "Creating sprite_2d for {}", entity);
						r.emplace<::kreogl::sprite_2d>(entity).tex = kreogl_texture;
					}
					if (r.all_of<sprite_3d>(entity)) {
						kengine_logf(r, verbose, log_category, "Creating sprite_3d for {}", entity);
						r.emplace<::kreogl::sprite_3d>(entity).tex = kreogl_texture;
					}
				}
			}

			for (auto [text_2d_entity, text_2d] : r.view<text_2d>(entt::exclude<::kreogl::text_2d>).each()) {
				kengine_logf(r, verbose, log_category, "Creating text_2d for {}", text_2d_entity);
				r.emplace<::kreogl::text_2d>(text_2d_entity);
			}

			for (auto [text_3d_entity, text_3d] : r.view<text_3d>(entt::exclude<::kreogl::text_3d>).each()) {
				kengine_logf(r, verbose, log_category, "Creating text_3d for {}", text_3d_entity);
				r.emplace<::kreogl::text_3d>(text_3d_entity);
			}

			for (auto [light_entity, light] : r.view<dir_light>(entt::exclude<::kreogl::directional_light>).each()) {
				kengine_logf(r, verbose, log_category, "Creating directional_light for {}", light_entity);
				r.emplace<::kreogl::directional_light>(light_entity);
			}

			for (auto [light_entity, light] : r.view<point_light>(entt::exclude<::kreogl::point_light>).each()) {
				kengine_logf(r, verbose, log_category, "Creating point_light for {}", light_entity);
				r.emplace<::kreogl::point_light>(light_entity);
			}

			for (auto [light_entity, light] : r.view<spot_light>(entt::exclude<::kreogl::spot_light>).each()) {
				kengine_logf(r, verbose, log_category, "Creating spot_light for {}", light_entity);
				r.emplace<::kreogl::spot_light>(light_entity);
			}

			for (auto [debug_entity, debug_graphics] : r.view<render::debug_graphics>(entt::exclude<kreogl::debug_graphics>).each()) {
				kengine_logf(r, verbose, log_category, "Creating debug_graphics for {}", debug_entity);
				r.emplace<kreogl::debug_graphics>(debug_entity);
			}
		}

		void tick_animations(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Ticking animations");

			const auto view = r.view<::kreogl::animated_object, animation::animation>();
			std::for_each(std::execution::par_unseq, putils_range(view), [&](entt::entity entity) noexcept {
				const putils::scoped_thread_name thread_name("Animation ticker");
				const auto & [kreogl_object, animation] = view.get(entity);
				tick_object_animation(delta_time, entity, kreogl_object, animation);
			});
		}

		void tick_object_animation(float delta_time, entt::entity entity, ::kreogl::animated_object & kreogl_object, animation::animation & animation) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Ticking animation for {}", entity);

			if (!kreogl_object.animation) {
				kengine_log(r, very_verbose, log_category, "No animation to play");
				return;
			}

			kreogl_object.tick_animation(delta_time);

			// Sync properties from kreogl_object to kengine components
			kengine_logf(r, very_verbose, log_category, "Syncing animation time and skeleton for {}", entity);
			animation.current_time = kreogl_object.animation->current_time;

			auto & skeleton = r.get<skeleton::skeleton>(entity);
			const auto nb_meshes = kreogl_object.skeleton.meshes.size();
			skeleton.meshes.resize(nb_meshes);
			for (size_t i = 0; i < nb_meshes; ++i) {
				const auto & kreogl_mesh = kreogl_object.skeleton.meshes[i];
				auto & mesh = skeleton.meshes[i];

				kengine_assert(r, kreogl_mesh.bone_mats_bone_space.size() < putils::lengthof(mesh.bone_mats_bone_space));
				std::ranges::copy(kreogl_mesh.bone_mats_bone_space, mesh.bone_mats_bone_space);
				kengine_assert(r, kreogl_mesh.bone_mats_mesh_space.size() < putils::lengthof(mesh.bone_mats_mesh_space));
				std::ranges::copy(kreogl_mesh.bone_mats_mesh_space, mesh.bone_mats_mesh_space);
			}
		}

		void draw() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Drawing");

			const auto view = r.view<::kreogl::window>();
			if (view.empty())
				return;

			update_imgui_scale();
			ImGui::Render();

			for (const auto & [window_entity, kreogl_window] : view.each()) {
				kengine_logf(r, very_verbose, log_category, "Drawing to {}", window_entity);
				kreogl_window.prepare_for_draw();
				draw_to_cameras(window_entity, kreogl_window);
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				kreogl_window.display();
			}

			end_imgui_frame();
		}

		float last_scale = 1.f;
		void update_imgui_scale() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Updating ImGui scale");

			const auto scale = imgui::get_scale(r);
			ImGui::GetIO().FontGlobalScale = scale;
			ImGui::GetStyle().ScaleAllSizes(scale / last_scale);
			last_scale = scale;
		}

		void end_imgui_frame() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Ending ImGui frame");

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				GLFWwindow * backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			if (!r.view<glfw::window>().empty())
				ImGui::NewFrame();
		}

		void draw_to_cameras(entt::entity window_entity, ::kreogl::window & kreogl_window) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [camera_entity, camera, viewport] : r.view<camera, viewport>().each()) {
				if (viewport.window != entt::null && viewport.window != window_entity) {
					kengine_logf(r, very_verbose, log_category, "Skipping camera {} because its viewport's window ({}) doesn't match", camera_entity, viewport.window);
					continue;
				}

				kengine_logf(r, very_verbose, log_category, "Drawing to camera {}", camera_entity);

				if (viewport.window == entt::null) {
					kengine_logf(r, verbose, log_category, "Setting target window for viewport in {}", camera_entity);
					viewport.window = window_entity;
					create_kreogl_camera(camera_entity, viewport);
				}

				auto & kreogl_camera = r.get<::kreogl::camera>(camera_entity);
				sync_camera_properties(kreogl_camera, camera, viewport);
				draw_to_camera(kreogl_window, camera_entity, kreogl_camera);
			}
		}

		void create_kreogl_camera(entt::entity camera_entity, viewport & viewport) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Creating camera for {}", camera_entity);

			const ::kreogl::camera::construction_params params{
				.viewport = {
					.resolution = toglm(viewport.resolution),
				}
			};
			auto & kreogl_camera = r.emplace<::kreogl::camera>(camera_entity, params);
			viewport.texture = render::viewport::render_texture(kreogl_camera.get_viewport().get_render_texture());
		}

		void sync_camera_properties(::kreogl::camera & kreogl_camera, const camera & camera, const viewport & viewport) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Syncing camera properties");

			kreogl_camera.set_position(toglm(camera.frustum.position));

			const auto facings = get_facings(camera);
			kreogl_camera.set_direction(toglm(facings.front));
			kreogl_camera.set_fov(camera.frustum.size.y);
			kreogl_camera.set_near_plane(camera.near_plane);
			kreogl_camera.set_far_plane(camera.far_plane);

			auto & kreogl_viewport = kreogl_camera.get_viewport_writable();
			kreogl_viewport.set_on_screen_position(toglm(viewport.bounding_box.position));
			kreogl_viewport.set_on_screen_size(toglm(viewport.bounding_box.size));
			kreogl_viewport.set_resolution(toglm(viewport.resolution));
			kreogl_viewport.set_z_order(viewport.z_order);
		}

		// Lazy-init these as OpenGL context doesn't exist upon construction, so can't init the shader
		std::optional<kreogl::highlight_shader> highlight_shader;
		std::optional<::kreogl::shader_pipeline> shader_pipeline;

		void draw_to_camera(::kreogl::window & kreogl_window, entt::entity camera_entity, const ::kreogl::camera & kreogl_camera) noexcept {
			KENGINE_PROFILING_SCOPE;

			::kreogl::world kreogl_world;
			sync_everything(kreogl_world, camera_entity);

			if (!shader_pipeline) {
				highlight_shader = kreogl::highlight_shader{ r };
				shader_pipeline = [this] {
					kengine_log(r, verbose, log_category, "Initializing shader pipeline");
					auto ret = ::kreogl::shader_pipeline::get_default_shaders();
					ret.add_shader(::kreogl::shader_step::post_process, *highlight_shader);
					return ret;
				}();
			}

			kreogl_window.draw_world_to_camera(kreogl_world, kreogl_camera, *shader_pipeline);
		}

		void sync_everything(::kreogl::world & kreogl_world, entt::entity camera_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing everything for camera {}", camera_entity);

			sync_all_objects(kreogl_world, camera_entity);
			sync_all_lights(kreogl_world, camera_entity);
		}

		void sync_common_properties(auto & kreogl_object, entt::entity entity, const instance::instance * instance, const core::transform & transform, const auto & colored_component, ::kreogl::world & kreogl_world) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing common properties for {}", entity);

			const core::transform * model_transform = nullptr;
			if (instance)
				model_transform = instance::try_get_model<core::transform>(r, *instance);
			kreogl_object.transform = glm::get_model_matrix(transform, model_transform);
			kreogl_object.color = toglm(colored_component.color);
			kreogl_object.user_data[0] = float(entity);

			kreogl_world.add(kreogl_object);
		};

		void sync_all_objects(::kreogl::world & kreogl_world, entt::entity camera_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing all objects for camera {}", camera_entity);

			for (const auto & [entity, instance, transform, drawable, kreogl_object] : r.view<instance::instance, core::transform, render::drawable, ::kreogl::animated_object>().each()) {
				if (!entity_appears_in_viewport(r, entity, camera_entity))
					continue;
				sync_common_properties(kreogl_object, entity, &instance, transform, drawable, kreogl_world);
				sync_animation_properties(kreogl_object, entity, instance);
				kreogl_object.cast_shadows = !r.all_of<no_shadow>(entity);
			}

			sync_sprite_2d_properties(kreogl_world, camera_entity);
			for (const auto & [sprite_entity, instance, transform, drawable, kreogl_sprite_3d] : r.view<instance::instance, core::transform, render::drawable, sprite_3d, ::kreogl::sprite_3d>().each()) {
				if (!entity_appears_in_viewport(r, sprite_entity, camera_entity))
					continue;
				sync_common_properties(kreogl_sprite_3d, sprite_entity, &instance, transform, drawable, kreogl_world);
			}

			sync_text_2d_properties(kreogl_world, camera_entity);
			for (const auto & [text_entity, transform, text_3d, kreogl_text_3d] : r.view<core::transform, text_3d, ::kreogl::text_3d>().each()) {
				if (!entity_appears_in_viewport(r, text_entity, camera_entity))
					continue;
				sync_text_properties(kreogl_text_3d, text_entity, transform, text_3d, kreogl_world);
			}

			sync_debug_graphics_properties(kreogl_world, camera_entity);

			for (const auto & [sky_box_entity, sky_box, instance] : r.view<sky_box, instance::instance>().each()) {
				if (!entity_appears_in_viewport(r, sky_box_entity, camera_entity))
					continue;
				const auto kreogl_skybox = instance::try_get_model<::kreogl::skybox_texture>(r, instance);
				if (!kreogl_skybox)
					continue;
				kreogl_world.skybox.color = toglm(sky_box.color);
				kreogl_world.skybox.texture = kreogl_skybox;
			}
		}

		void sync_animation_properties(::kreogl::animated_object & kreogl_object, entt::entity entity, const instance::instance & instance) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing animation properties for {}", entity);

			const auto animation = r.try_get<render::animation::animation>(entity);
			if (!animation)
				return;

			const auto animation_model = find_animation_model(instance, animation->current_anim);
			if (!animation_model)
				kreogl_object.animation.reset();
			else {
				kreogl_object.animation = ::kreogl::animation{
					.model = animation_model,
					.current_time = animation->current_time,
					.speed = animation->speed,
					.loop = animation->loop
				};

				static const std::unordered_map<render::animation::animation::mover_behavior, ::kreogl::animation::mover_behavior> mover_behaviors = {
					{ render::animation::animation::mover_behavior::update_transform_component, ::kreogl::animation::mover_behavior::update_transform },
					{ render::animation::animation::mover_behavior::update_bones, ::kreogl::animation::mover_behavior::update_bones },
					{ render::animation::animation::mover_behavior::none, ::kreogl::animation::mover_behavior::none }
				};

				kreogl_object.animation->position_mover_behavior = mover_behaviors.at(animation->position_mover_behavior);
				kreogl_object.animation->rotation_mover_behavior = mover_behaviors.at(animation->rotation_mover_behavior);
				kreogl_object.animation->scale_mover_behavior = mover_behaviors.at(animation->scale_mover_behavior);
			}
		}

		const ::kreogl::animation_model * find_animation_model(const instance::instance & instance, size_t animation_index) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Finding animation model");

			const auto & kreogl_model = instance::get_model<model>(r, instance);
			if (animation_index < kreogl_model.ptr->animations->animations.size()) {
				kengine_log(r, very_verbose, log_category, "Found animation in model");
				return kreogl_model.ptr->animations->animations[animation_index].get();
			}

			kengine_log(r, very_verbose, log_category, "Animation isn't in model, looking in animation_files");

			const auto kreogl_model_animation_files = instance::try_get_model<animation_files>(r, instance);
			if (!kreogl_model_animation_files) {
				kengine_log(r, very_verbose, log_category, "No animation_files, animation not found");
				return nullptr;
			}

			size_t skipped_animations = kreogl_model.ptr->animations->animations.size();
			for (const auto & file : kreogl_model_animation_files->files) {
				const auto index_in_file = animation_index - skipped_animations;
				if (index_in_file < file->animations.size()) {
					kengine_log(r, very_verbose, log_category, "Found animation in animation_files");
					return file->animations[index_in_file].get();
				}
				skipped_animations += file->animations.size();
			}

			kengine_log(r, very_verbose, log_category, "Animation not found");
			return nullptr;
		}

		void sync_sprite_2d_properties(::kreogl::world & kreogl_world, entt::entity camera_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing sprite_2d properties for camera {}", camera_entity);

			for (const auto & [sprite_entity, instance, transform, drawable, sprite_2d, kreogl_sprite_2d] : r.view<instance::instance, core::transform, render::drawable, sprite_2d, ::kreogl::sprite_2d>().each()) {
				if (!entity_appears_in_viewport(r, sprite_entity, camera_entity))
					continue;

				sync_common_properties(kreogl_sprite_2d, sprite_entity, &instance, transform, drawable, kreogl_world);

				kengine_logf(r, very_verbose, log_category, "Syncing sprite_2d properties for {}", sprite_entity);
				const auto & viewport = r.get<render::viewport>(camera_entity);
				const auto & box = convert_to_screen_percentage(transform.bounding_box, viewport.resolution, sprite_2d);
				kreogl_sprite_2d.transform = get_on_screen_matrix(transform, box.position, box.size, sprite_2d);
			}
		}

		void sync_text_2d_properties(::kreogl::world & kreogl_world, entt::entity camera_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing text_2d properties for camera {}", camera_entity);

			for (const auto & [text_entity, transform, text_2d, kreogl_text_2d] : r.view<core::transform, text_2d, ::kreogl::text_2d>().each()) {
				if (!entity_appears_in_viewport(r, text_entity, camera_entity))
					continue;

				sync_text_properties(kreogl_text_2d, text_entity, transform, text_2d, kreogl_world);

				kengine_logf(r, very_verbose, log_category, "Syncing text_2d properties for {}", text_entity);
				const auto & viewport = r.get<render::viewport>(camera_entity);
				const auto & box = convert_to_screen_percentage(transform.bounding_box, viewport.resolution, text_2d);
				auto scale = transform.bounding_box.size.y;
				switch (text_2d.coordinates) {
					case text_2d::coordinate_type::pixels:
						scale /= viewport.resolution.y;
						break;
					case text_2d::coordinate_type::screen_percentage:
					default:
						static_assert(magic_enum::enum_count<text_2d::coordinate_type>() == 2);
						break;
				}
				kreogl_text_2d.transform = get_on_screen_matrix(transform, box.position, { scale, scale, scale }, text_2d);
			}
		}

		void sync_debug_graphics_properties(::kreogl::world & kreogl_world, entt::entity camera_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing debug_graphics properties for camera {}", camera_entity);

			for (const auto & [debug_entity, transform, debug_graphics, kreogl_debug_graphics] : r.view<core::transform, render::debug_graphics, kreogl::debug_graphics>().each()) {
				if (!entity_appears_in_viewport(r, debug_entity, camera_entity))
					continue;

				kengine_logf(r, very_verbose, log_category, "Syncing debug_graphics properties for {}", debug_entity);
				kreogl_debug_graphics.elements.resize(debug_graphics.elements.size());
				for (size_t i = 0; i < debug_graphics.elements.size(); ++i) {
					const auto & element = debug_graphics.elements[i];
					auto & kreogl_element = kreogl_debug_graphics.elements[i];
					kreogl_element.color = toglm(element.color);
					kreogl_element.user_data[0] = float(debug_entity);

					kreogl_element.transform = ::glm::mat4{ 1.f };
					switch (element.type) {
						case render::debug_graphics::element_type::line:
							kreogl_element.type = ::kreogl::debug_element::type::line;
							kreogl_element.line_start = toglm(element.pos);
							kreogl_element.line_end = toglm(element.line.end);
							break;
						case render::debug_graphics::element_type::box:
							kreogl_element.type = ::kreogl::debug_element::type::box;
							apply_debug_graphics_transform(kreogl_element.transform, transform, toglm(element.pos), toglm(element.box.size), element.relative_to);
							break;
						case render::debug_graphics::element_type::sphere:
							kreogl_element.type = ::kreogl::debug_element::type::sphere;
							apply_debug_graphics_transform(kreogl_element.transform, transform, toglm(element.pos), ::glm::vec3(element.sphere.radius), element.relative_to);
							break;
						default:
							static_assert(magic_enum::enum_count<render::debug_graphics::element_type>() == 3); // Exhaustive switch
							kengine_assert_failed(r, "Non-exhaustive switch");
							break;
					}

					kreogl_world.add(kreogl_element);
				}
			}
		}

		void apply_debug_graphics_transform(::glm::mat4 & matrix, const core::transform & transform, const ::glm::vec3 & pos, const ::glm::vec3 & size, render::debug_graphics::reference_space reference_space) noexcept {
			KENGINE_PROFILING_SCOPE;

			matrix = ::glm::translate(matrix, pos);
			if (reference_space == render::debug_graphics::reference_space::object) {
				matrix = ::glm::translate(matrix, toglm(transform.bounding_box.position));
				matrix = ::glm::rotate(matrix, transform.yaw, { 0.f, 1.f, 0.f });
				matrix = ::glm::rotate(matrix, transform.pitch, { 1.f, 0.f, 0.f });
				matrix = ::glm::rotate(matrix, transform.roll, { 0.f, 0.f, 1.f });
			}
			matrix = ::glm::scale(matrix, size);
			if (reference_space == render::debug_graphics::reference_space::object)
				matrix = ::glm::scale(matrix, toglm(transform.bounding_box.size));
		}

		::glm::mat4 get_on_screen_matrix(const core::transform & transform, const putils::point3f & position, const putils::point3f & size, const on_screen & onScreen) noexcept {
			KENGINE_PROFILING_SCOPE;

			::glm::mat4 model{ 1.f };
			// convert NDC to [0,1]
			model = ::glm::translate(model, { -1.f, -1.f, 0.f });
			const auto centre = 2.f * ::glm::vec3{ position.x, 1.f - position.y, position.z };

			model = ::glm::translate(model, centre);
			model = ::glm::scale(model, toglm(size));
			model = ::glm::rotate(model, transform.yaw, { 0.f, 1.f, 0.f });
			model = ::glm::rotate(model, transform.pitch, { 1.f, 0.f, 0.f });
			model = ::glm::rotate(model, transform.roll, { 0.f, 0.f, 1.f });
			return model;
		}

		void sync_text_properties(auto & kreogl_text, entt::entity text_entity, const core::transform & transform, const text & text, ::kreogl::world & kreogl_world) noexcept {
			KENGINE_PROFILING_SCOPE;

			sync_common_properties(kreogl_text, text_entity, nullptr, transform, text, kreogl_world);
			kreogl_text.font = text.font;
			kreogl_text.value = text.value;
			kreogl_text.font_size = text.font_size;
			switch (text.alignment) {
				case text::alignment_type::left:
					kreogl_text.alignment = ::kreogl::text::alignment_type::left;
					break;
				case text::alignment_type::right:
					kreogl_text.alignment = ::kreogl::text::alignment_type::right;
					break;
				case text::alignment_type::center:
					kreogl_text.alignment = ::kreogl::text::alignment_type::center;
					break;
				default:
					static_assert(magic_enum::enum_count<text::alignment_type>() == 3); // Exhaustive switch
					kengine_assert_failed(r, "Non-exhaustive switch");
					break;
			}
		}

		void sync_all_lights(::kreogl::world & kreogl_world, entt::entity camera_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing all lights for camera {}", camera_entity);

			sync_all_dir_lights(kreogl_world, camera_entity);
			sync_all_point_lights(kreogl_world, camera_entity);
			sync_all_spot_lights(kreogl_world, camera_entity);
		}

		void sync_all_dir_lights(::kreogl::world & kreogl_world, entt::entity camera_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing all directional lights for camera {}", camera_entity);

			for (const auto & [light_entity, dir_light, kreogl_dir_light] : r.view<dir_light, ::kreogl::directional_light>().each()) {
				if (!entity_appears_in_viewport(r, light_entity, camera_entity))
					continue;

				sync_light_properties(light_entity, kreogl_dir_light, dir_light, kreogl_world);

				kengine_logf(r, very_verbose, log_category, "Syncing directional light properties for {}", light_entity);
				kreogl_dir_light.direction = toglm(dir_light.direction);
				kreogl_dir_light.light_sphere_distance = dir_light.light_sphere_distance;
				kreogl_dir_light.ambient_strength = dir_light.ambient_strength;

				kreogl_dir_light.cascade_ends.clear();
				for (const auto cascade_end : dir_light.cascade_ends)
					kreogl_dir_light.cascade_ends.push_back(cascade_end);
				kreogl_dir_light.shadow_caster_max_distance = dir_light.shadow_caster_max_distance;
			}
		}

		void sync_all_point_lights(::kreogl::world & kreogl_world, entt::entity camera_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing all point lights for camera {}", camera_entity);

			for (const auto & [light_entity, transform, point_light, kreogl_point_light] : r.view<core::transform, point_light, ::kreogl::point_light>().each()) {
				if (!entity_appears_in_viewport(r, light_entity, camera_entity))
					continue;
				sync_point_light_properties(light_entity, transform, kreogl_point_light, point_light, kreogl_world);
			}
		}

		void sync_all_spot_lights(::kreogl::world & kreogl_world, entt::entity camera_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing all spot lights for camera {}", camera_entity);

			for (const auto & [light_entity, transform, spot_light, kreogl_spot_light] : r.view<core::transform, spot_light, ::kreogl::spot_light>().each()) {
				if (!entity_appears_in_viewport(r, light_entity, camera_entity))
					continue;

				sync_point_light_properties(light_entity, transform, kreogl_spot_light, spot_light, kreogl_world);

				kengine_logf(r, very_verbose, log_category, "Syncing spot light properties for {}", light_entity);
				kreogl_spot_light.direction = toglm(spot_light.direction);
				kreogl_spot_light.cut_off = spot_light.cut_off;
				kreogl_spot_light.outer_cut_off = spot_light.outer_cut_off;
			}
		}

		void sync_light_properties(entt::entity light_entity, auto & kreogl_light, const light & light, ::kreogl::world & kreogl_world) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing light properties for {}", light_entity);

			kreogl_light.color = toglm(light.color);
			kreogl_light.diffuse_strength = light.diffuse_strength;
			kreogl_light.specular_strength = light.specular_strength;
			kreogl_light.cast_shadows = light.cast_shadows;
			kreogl_light.shadow_pcf_samples = light.shadow_pcf_samples;
			kreogl_light.shadow_map_size = light.shadow_map_size;
			kreogl_light.shadow_map_max_bias = light.shadow_map_max_bias;
			kreogl_light.shadow_map_min_bias = light.shadow_map_min_bias;

			if (const auto god_rays = r.try_get<render::god_rays>(light_entity)) {
				if (!kreogl_light.volumetric_lighting)
					kreogl_light.volumetric_lighting.emplace();
				kreogl_light.volumetric_lighting->default_step_length = god_rays->default_step_length;
				kreogl_light.volumetric_lighting->intensity = god_rays->intensity;
				kreogl_light.volumetric_lighting->nb_steps = god_rays->nb_steps;
				kreogl_light.volumetric_lighting->scattering = god_rays->scattering;
			}
			else
				kreogl_light.volumetric_lighting.reset();

			kreogl_world.add(kreogl_light);
		}

		void sync_point_light_properties(entt::entity light_entity, const core::transform & transform, ::kreogl::point_light & kreogl_light, const point_light & light, ::kreogl::world & kreogl_world) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Syncing point light properties for {}", light_entity);

			sync_light_properties(light_entity, kreogl_light, light, kreogl_world);

			kreogl_light.position = toglm(transform.bounding_box.position);
			kreogl_light.attenuation_constant = light.attenuation_constant;
			kreogl_light.attenuation_linear = light.attenuation_linear;
			kreogl_light.attenuation_quadratic = light.attenuation_quadratic;
		}

		entt::entity get_entity_in_pixel(entt::entity window_id, const putils::point2ui & pixel) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Getting entity in {} of {}", pixel, window_id);

			const auto entity_id_in_pixel = read_from_gbuffer(window_id, pixel, ::kreogl::gbuffer::texture::user_data);
			if (!entity_id_in_pixel)
				return entt::null;

			const auto ret = entt::entity(entity_id_in_pixel->r);
			if (ret == entt::entity(entt::id_type(0))) {
				kengine_log(r, verbose, log_category, "Found no entity");
				return entt::null;
			}

			kengine_logf(r, verbose, log_category, "Found {}", ret);
			return ret;
		}

		std::optional<putils::point3f> get_position_in_pixel(entt::entity window, const putils::point2ui & pixel) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Getting position in {} of {}", pixel, window);

			// Check that an entity was drawn in the pixel, otherwise position buffer is invalid
			if (get_entity_in_pixel(window, pixel) == entt::null)
				return std::nullopt;

			const auto position_in_pixel = read_from_gbuffer(window, pixel, ::kreogl::gbuffer::texture::position);
			if (!position_in_pixel) {
				kengine_log(r, verbose, log_category, "Found no position");
				return std::nullopt;
			}

			kengine_logf(r, verbose, log_category, "Found {}", *position_in_pixel);
			return &position_in_pixel->x;
		}

		std::optional<::glm::vec4> read_from_gbuffer(entt::entity window, const putils::point2ui & pixel, ::kreogl::gbuffer::texture texture) noexcept {
			KENGINE_PROFILING_SCOPE;

			static constexpr auto GBUFFER_TEXTURE_COMPONENTS = 4;

			if (window == entt::null) {
				for (const auto & [window_entity, kreogl_window] : r.view<::kreogl::window>().each()) {
					window = window_entity;
					break;
				}

				if (window == entt::null) {
					kengine_log(r, verbose, log_category, "No existing Kreogl window");
					return std::nullopt;
				}
			}

			if (!r.all_of<::kreogl::window>(window)) {
				kengine_logf(r, verbose, log_category, "{} does not have a Kreogl window", window);
				return std::nullopt;
			}

			const auto viewport_info = get_viewport_for_pixel({ r, window }, pixel);
			if (viewport_info.camera == entt::null) {
				kengine_logf(r, verbose, log_category, "Found no viewport containing pixel {}", pixel);
				return std::nullopt;
			}

			const auto camera_entity = viewport_info.camera;
			const auto kreogl_camera = r.try_get<::kreogl::camera>(camera_entity);
			if (!kreogl_camera) {
				kengine_logf(r, verbose, log_category, "Viewport {} does not have a kreogl::camera", camera_entity);
				return std::nullopt;
			}

			const auto & viewport = kreogl_camera->get_viewport();
			const auto & gbuffer = viewport.get_gbuffer();
			const auto gbuffer_size = viewport.get_resolution();

			const auto pixel_in_gbuffer = ::glm::vec2(gbuffer_size) * toglm(viewport_info.viewport_percent);
			if (pixel_in_gbuffer.x >= float(gbuffer_size.x) || pixel_in_gbuffer.y > float(gbuffer_size.y) || pixel_in_gbuffer.y == 0) {
				kengine_logf(r, verbose, log_category, "Pixel is out of {}'s gbuffer's bounds", camera_entity);
				return std::nullopt;
			}

			return gbuffer.read_pixel(texture, pixel_in_gbuffer);
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed_animation_files,
		system::processed_model,
		system::processed_sky_box,
		system::processed_window,
		animation_files,
		debug_graphics,
		model,
		::kreogl::animated_object,
		::kreogl::camera,
		::kreogl::directional_light,
		::kreogl::point_light,
		::kreogl::skybox_texture,
		::kreogl::spot_light,
		::kreogl::sprite_2d,
		::kreogl::sprite_3d,
		::kreogl::text_2d,
		::kreogl::text_3d,
		::kreogl::texture,
		::kreogl::window
	)
}