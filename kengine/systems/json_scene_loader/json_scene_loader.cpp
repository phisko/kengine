#include "json_scene_loader.hpp"

// stl
#include <filesystem>
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/observer.hpp>
#include <entt/entity/registry.hpp>

// nlohmann
#include <nlohmann/json.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/json_scene_loader.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/async_helper.hpp"
#include "kengine/helpers/json_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	struct json_scene_loader {
		entt::registry & r;

		struct task {};

		json_scene_loader(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			for (const auto & [loader_entity, loader] : r.view<data::json_scene_loader>().each())
				load_temporary_scene(loader_entity, loader);
		}

		struct temporary_scene {
			std::vector<entt::entity> loaded_entities;
		};

		struct load_models_task {};
		struct load_scene_task {};

		entt::observer observer{ r, entt::collector.group<data::json_scene_loader>() };
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto e : observer) {
				const auto & loader = r.get<data::json_scene_loader>(e);
				load_temporary_scene(e, loader);
			}
			observer.clear();

			kengine::process_async_results<temporary_scene>(r, [this](entt::entity e, temporary_scene && scene) {
				r.emplace<temporary_scene>(e, std::move(scene));
				const auto & loader = r.get<data::json_scene_loader>(e);
				load_models(e, loader);
			});

			kengine::process_async_results<load_models_task>(r, [this](entt::entity e, load_models_task &&) {
				// Entity that will wait until all async loading tasks are completed before loading the scene
				const auto poller = r.create();
				r.emplace<functions::execute>(poller, [this, e, poller](float delta_time) {
					if (!r.view<data::async_task>().empty())
						return;
					const auto & loader = r.get<data::json_scene_loader>(e);
					load_scene(e, loader);
					r.destroy(poller);
				});
			});

			kengine::process_async_results<load_scene_task>(r, [this](entt::entity e, load_scene_task &&) {
				if (const auto scene = r.try_get<temporary_scene>(e))
					r.destroy(scene->loaded_entities.begin(), scene->loaded_entities.end());
				r.destroy(e);
			});
		}

		template<typename Result, typename Func>
		void perform_step(entt::entity e, const data::json_scene_loader::loading_step & step, const char * description, Func && step_func) noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine::start_async_task<Result>(
				r, e,
				data::async_task::string("json_scene: load %s %s", description, step.file.c_str()),
				std::async(step.policy, FWD(step_func))
			);
		}

		void load_temporary_scene(entt::entity e, const data::json_scene_loader & loader) noexcept {
			KENGINE_PROFILING_SCOPE;

			perform_step<temporary_scene>(
				e, loader.temporary_scene, "temporary scene",
				[this, &loader] {
					return load_temporary_scene(loader.temporary_scene.file.c_str());
				}
			);
		}

		void load_models(entt::entity e, const data::json_scene_loader & loader) noexcept {
			KENGINE_PROFILING_SCOPE;

			perform_step<load_models_task>(
				e, loader.model_directory, "models from",
				[this, e, &loader] {
					return load_models(loader.model_directory.file.c_str());
				}
			);
		}

		void load_scene(entt::entity e, const data::json_scene_loader & loader) noexcept {
			KENGINE_PROFILING_SCOPE;

			perform_step<load_scene_task>(
				e, loader.scene, "scene",
				[this, e, &loader] {
					return load_scene(loader.scene.file.c_str());
				}
			);
		}

		temporary_scene load_temporary_scene(const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;

			temporary_scene scene;
			if (file[0] == 0)
				return scene;

			std::ifstream f(file);
			const auto temporary_scene_json = nlohmann::json::parse(f);

			for (const auto & json : temporary_scene_json) {
				const auto e = r.create();
				scene.loaded_entities.push_back(e);
				json_helper::load_entity(json, { r, e });
			}

			return scene;
		}

		load_models_task load_models(const char * dir) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (dir[0] == 0)
				return {};

			for (const auto & entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (entry.path().extension() != ".json")
					continue;

				const auto e = r.create();
				std::ifstream f(entry.path());
				json_helper::load_entity(nlohmann::json::parse(f), { r, e });
			}

			return {};
		}

		load_scene_task load_scene(const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (file[0] == 0)
				return {};

			std::ifstream f(file);
			const auto scene_json = nlohmann::json::parse(f);

			for (const auto & json : scene_json) {
				const auto e = r.create();
				json_helper::load_entity(json, { r, e });
			}

			return {};
		}
	};

	entt::entity add_json_scene_loader(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<json_scene_loader>(e);
		return e;
	}
}
