#include "json_scene_loader.hpp"

// stl
#include <concepts>
#include <filesystem>
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// nlohmann
#include <nlohmann/json.hpp>

// putils
#include "putils/forward_to.hpp"
#include "putils/range.hpp"

// kengine core
#include "kengine/core/helpers/json_helper.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine helpers
#include "kengine/async/helpers/async_helper.hpp"

// kengine json_scene_loader
#include "kengine/json_scene_loader/data/json_scene_loader.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

namespace kengine::systems {
	struct json_scene_loader {
		entt::registry & r;

		struct processed {};
		kengine::new_entity_processor<processed, data::json_scene_loader> processor{ r, putils_forward_to_this(process_new_loader) };

		json_scene_loader(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "json_scene_loader", "Initializing");

			e.emplace<functions::execute>(putils_forward_to_this(execute));
			processor.process();
		}

		struct temporary_scene {
			std::vector<entt::entity> loaded_entities;
		};
		struct load_models_task {};
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "json_scene_loader", "Executing");

			processor.process();

			kengine::process_async_results<load_models_task>(r, [this](entt::entity e, load_models_task &&) {
				// Entity that will wait until all async loading tasks are completed before loading the scene
				const auto poller = r.create();
				kengine_logf(r, verbose, "json_scene_loader", "Creating polling entity [%u]", poller);
				r.emplace<functions::execute>(poller, [this, e, poller](float delta_time) {
					if (!r.view<data::async_task>().empty()) {
						kengine_log(r, verbose, "json_scene_loader", "Waiting to load scene (async_tasks are still running)");
						return;
					}

					kengine_log(r, log, "json_scene_loader", "Destroying temporary scene");
					const auto & scene = r.get<temporary_scene>(e);
					r.destroy(putils_range(scene.loaded_entities));

					const auto & loader = r.get<data::json_scene_loader>(e);
					load_scene(loader.scene.c_str());

					kengine_logf(r, verbose, "json_scene_loader", "Destroying polling entity [%u]", poller);
					r.destroy(poller);
				});
			});
		}

		void process_new_loader(entt::entity e, const data::json_scene_loader & loader) noexcept {
			kengine_logf(r, verbose, "json_scene_loader", "Processing new scene loader [%u]", e);

			if (loader.model_directory.empty()) {
				if (!loader.scene.empty())
					load_scene(loader.scene.c_str());
				else
					kengine_logf(r, warning, "json_scene_loader", "Empty scene loader found in [%u]", e);
				return;
			}

			r.emplace<temporary_scene>(e, load_temporary_scene(loader.temporary_scene.c_str()));
			start_async_model_loading(e, loader);
		}

		void start_async_model_loading(entt::entity e, const data::json_scene_loader & loader) noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine::start_async_task(
				r, e,
				data::async_task::string("json_scene_loader: load models from %s", loader.model_directory.c_str()),
				std::async(std::launch::async, [this, &loader] {
					return load_models(loader.model_directory.c_str());
				})
			);
		}

		temporary_scene load_temporary_scene(const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;

			temporary_scene scene;
			if (file[0] == 0) {
				kengine_log(r, warning, "json_scene_loader", "No temporary scene specified, consider adding a loading screen");
				return scene;
			}

			kengine_logf(r, log, "json_scene_loader", "Loading temporary scene from %s", file);

			std::ifstream f(file);
			const auto temporary_scene_json = nlohmann::json::parse(f);

			for (const auto & json : temporary_scene_json) {
				const auto e = r.create();
				kengine_logf(r, verbose, "json_scene_loader", "Creating temporary [%u]", e);
				scene.loaded_entities.push_back(e);
				json_helper::load_entity(json, { r, e });
			}

			kengine_log(r, log, "json_scene_loader", "Temporary scene loaded");

			return scene;
		}

		load_models_task load_models(const char * dir) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (entry.path().extension() != ".json")
					continue;

				const auto e = r.create();
				std::ifstream f(entry.path());
				const auto json = nlohmann::json::parse(f);
				kengine_logf(r, verbose, "json_scene_loader", "Creating model [%u]", e);
				json_helper::load_entity(json, { r, e });
			}

			return {};
		}

		void load_scene(const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, log, "json_scene_loader", "Loading scene from %s", file);

			std::ifstream f(file);
			const auto scene_json = nlohmann::json::parse(f);

			for (const auto & json : scene_json) {
				const auto e = r.create();
				kengine_logf(r, verbose, "json_scene_loader", "Creating [%u]", e);
				json_helper::load_entity(json, { r, e });
			}

			kengine_log(r, log, "json_scene_loader", "Scene loaded");
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		json_scene_loader,
		json_scene_loader::processed,
		json_scene_loader::temporary_scene
	)
}
