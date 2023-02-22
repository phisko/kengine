#include "system.hpp"

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
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine meta/json
#include "kengine/meta/json/helpers/load_entity.hpp"

// kengine helpers
#include "kengine/async/helpers/start_task.hpp"
#include "kengine/async/helpers/process_results.hpp"

// kengine json_scene_loader
#include "kengine/json_scene_loader/data/request.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

namespace kengine::json_scene_loader {
	struct system {
		static constexpr auto log_category = "json_scene_loader";
		entt::registry & r;

		struct processed {};
		kengine::new_entity_processor<processed, request> processor{ r, putils_forward_to_this(process_new_request) };

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));
			processor.process();
		}

		struct temporary_scene {
			std::vector<entt::entity> loaded_entities;
		};
		struct load_models_task {};
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			processor.process();

			kengine::async::process_results<load_models_task>(r, [this](entt::entity e, load_models_task &&) {
				// Entity that will wait until all async loading tasks are completed before loading the scene
				const auto poller = r.create();
				kengine_logf(r, verbose, log_category, "Creating polling entity [%u]", poller);
				r.emplace<main_loop::execute>(poller, [this, e, poller](float delta_time) {
					if (!r.view<async::task>().empty()) {
						kengine_log(r, verbose, log_category, "Waiting to load scene (async_tasks are still running)");
						return;
					}

					kengine_log(r, log, log_category, "Destroying temporary scene");
					const auto & scene = r.get<temporary_scene>(e);
					r.destroy(putils_range(scene.loaded_entities));

					const auto & comp = r.get<request>(e);
					load_scene(comp.scene.c_str());

					kengine_logf(r, verbose, log_category, "Destroying polling entity [%u]", poller);
					r.destroy(poller);
				});
			});
		}

		void process_new_request(entt::entity e, const request & comp) noexcept {
			kengine_logf(r, verbose, log_category, "Processing new request [%u]", e);

			if (comp.model_directory.empty()) {
				if (!comp.scene.empty())
					load_scene(comp.scene.c_str());
				else
					kengine_logf(r, warning, log_category, "Empty request found in [%u]", e);
				return;
			}

			r.emplace<temporary_scene>(e, load_temporary_scene(comp.temporary_scene.c_str()));
			start_async_model_loading(e, comp);
		}

		void start_async_model_loading(entt::entity e, const request & comp) noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine::async::start_task(
				r, e,
				async::task::string("json_scene_loader: load models from %s", comp.model_directory.c_str()),
				std::async(std::launch::async, [this, &comp] {
					return load_models(comp.model_directory.c_str());
				})
			);
		}

		temporary_scene load_temporary_scene(const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;

			temporary_scene scene;
			if (file[0] == 0) {
				kengine_log(r, warning, log_category, "No temporary scene specified, consider adding a loading screen");
				return scene;
			}

			kengine_logf(r, log, log_category, "Loading temporary scene from %s", file);

			std::ifstream f(file);
			if (!f) {
				kengine_logf(r, error, log_category, "Failed to open %s", file);
				return scene;
			}

			const auto temporary_scene_json = nlohmann::json::parse(f);

			for (const auto & json : temporary_scene_json) {
				const auto e = r.create();
				kengine_logf(r, verbose, log_category, "Creating temporary [%u]", e);
				scene.loaded_entities.push_back(e);
				meta::json::load_entity(json, { r, e });
			}

			kengine_log(r, log, log_category, "Temporary scene loaded");

			return scene;
		}

		load_models_task load_models(const char * dir) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!std::filesystem::is_directory(dir)) {
				kengine_logf(r, error, log_category, "%s is not a directory", dir);
				return {};
			}

			for (const auto & entry : std::filesystem::recursive_directory_iterator(dir)) {
				if (entry.path().extension() != ".json")
					continue;

				const auto e = r.create();
				std::ifstream f(entry.path());
				const auto json = nlohmann::json::parse(f);
				kengine_logf(r, verbose, log_category, "Creating model [%u]", e);
				meta::json::load_entity(json, { r, e });
			}

			return {};
		}

		void load_scene(const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, log, log_category, "Loading scene from %s", file);

			std::ifstream f(file);
			if (!f) {
				kengine_logf(r, error, log_category, "Failed to open %s", file);
				return;
			}

			const auto scene_json = nlohmann::json::parse(f);

			for (const auto & json : scene_json) {
				const auto e = r.create();
				kengine_logf(r, verbose, log_category, "Creating [%u]", e);
				meta::json::load_entity(json, { r, e });
			}

			kengine_log(r, log, log_category, "Scene loaded");
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed,
		system::temporary_scene
	)
}
