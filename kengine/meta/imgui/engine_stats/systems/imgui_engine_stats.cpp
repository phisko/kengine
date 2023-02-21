#include "imgui_engine_stats.hpp"

// stl
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// nlohmann
#include <nlohmann/json.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"

// kengine core
#include "kengine/core/data/name.hpp"
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/core/sort/helpers/get_name_sorted_entities.hpp"

// kengine imgui/imgui_tool
#include "kengine/imgui/tool/data/tool.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

// kengine meta
#include "kengine/meta/functions/count.hpp"
#include "kengine/meta/functions/has.hpp"

#ifndef KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE
#define KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE "tracked_entity_collections.json"
#endif

namespace kengine::systems {
	struct imgui_engine_stats {
		entt::registry & r;
		bool * enabled = nullptr;

		imgui_engine_stats(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "imgui_engine_stats", "Initializing");

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			e.emplace<core::name>("Entities/Stats");
			auto & tool = e.emplace<imgui::tool::tool>();
			enabled = &tool.enabled;
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "imgui_engine_stats", "Executing");

			if (!*enabled) {
				kengine_log(r, very_verbose, "imgui_engine_stats", "Disabled");
				return;
			}

			if (ImGui::Begin("Engine stats", enabled)) {
				ImGui::Text("Entities: %zu", r.alive());
				ImGui::Text("\tEntity pool size: %zu", r.size());
				const auto component_count = std::ranges::count_if(r.storage(), [](auto &&) { return true; });
				ImGui::Text("Component types: %zu", component_count);
				display_tracked_collections();
			}
			ImGui::End();
		}

		struct collection {
			std::string name;
			std::vector<entt::entity> components;
			std::vector<std::string> missing_components;
		};
		std::vector<collection> tracked = load_tracked_collections();
		void display_tracked_collections() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "imgui_engine_stats", "Displaying tracked collections");

			const auto new_collection = display_collection_creator();
			if (new_collection) {
				kengine_logf(r, log, "imgui_engine_stats", "New tracked collection: %s", new_collection->name.c_str());
				tracked.push_back(*new_collection);
				save_tracked_collections(tracked);
			}

			for (size_t i = 0; i < tracked.size(); ++i) {
				auto & collection = tracked[i];
				kengine_logf(r, very_verbose, "imgui_engine_stats", "Displaying tracked collection %s", collection.name.c_str());

				if (collection.missing_components.empty()) {
					kengine_log(r, very_verbose, "imgui_engine_stats", "No missing components, displaying count");
					const auto count = get_collection_count(collection);
					ImGui::Text("%s: %zu", collection.name.c_str(), count);
				}
				else {
					std::string missing_components;

					bool first = true;
					for (const auto & s : collection.missing_components) {
						if (first)
							missing_components += ", ";
						first = false;
						missing_components += s;
					}

					kengine_logf(r, very_verbose, "imgui_engine_stats", "Missing components %s", missing_components.c_str());
					ImGui::Text("%s: missing Component Entities for %s", collection.name.c_str(), missing_components.c_str());
					find_missing_component_entities(collection);
				}

				if (ImGui::BeginPopupContextItem("Remove collection")) {
					if (ImGui::MenuItem("Remove")) {
						kengine_log(r, verbose, "imgui_engine_stats", "Removing tracked collection");
						tracked.erase(tracked.begin() + i);
						--i;
					}
					ImGui::EndPopup();
				}
			}
		}

		collection creating;
		std::optional<collection> display_collection_creator() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "imgui_engine_stats", "Displaying collection creator");

			if (ImGui::Button("Track new collection", { -1.f, 0.f }))
				ImGui::OpenPopup("Create collection");

			if (ImGui::BeginPopup("Create collection")) {
				if (ImGui::Button("Track", { -1.f, 0.f })) {
					ImGui::CloseCurrentPopup();
					if (!creating.components.empty()) {
						collection ret;
						std::swap(ret, creating);

						bool first = true;
						for (const auto id : ret.components) {
							if (!first)
								ret.name += " + ";
							ret.name += r.get<core::name>(id).name;
							first = false;
						}

						ImGui::EndPopup();
						return ret;
					}
				}

				const auto sorted_entities = core::sort::get_name_sorted_entities<meta::has>(r);

				for (const auto [e, name, has] : sorted_entities) {
					const auto it = std::ranges::find(creating.components, e);
					bool in_creating = it != creating.components.end();
					if (ImGui::Checkbox(name->name.c_str(), &in_creating)) {
						kengine_logf(r, verbose, "imgui_engine_stats", "Adding component %s to potential new collection", name->name.c_str());
						if (in_creating)
							creating.components.push_back(e);
						else
							creating.components.erase(it);
					}
				}
				ImGui::EndPopup();
			}

			return std::nullopt;
		}

		size_t get_collection_count(const collection & collection) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, "imgui_engine_stats", "Getting entity count for collection %s", collection.name.c_str());

			if (collection.components.size() == 1) {
				const auto comp = collection.components[0];
				const auto count = r.try_get<meta::count>(comp);
				if (count)
					return count->call(r);
			}

			size_t ret = 0;
			r.each([&](entt::entity e) {
				bool good = true;

				for (const auto comp : collection.components) {
					const auto & has = r.get<meta::has>(comp);
					if (!has({ r, e })) {
						kengine_logf(r, very_verbose, "imgui_engine_stats", "Discarding entity [%u] because it doesn't have %s", e, r.get<core::name>(comp).name.c_str());
						good = false;
						break;
					}
				}

				if (good) {
					kengine_logf(r, very_verbose, "imgui_engine_stats", "Found entity [%u]", e);
					++ret;
				}
			});

			return ret;
		}

		void save_tracked_collections(const std::vector<collection> & collections) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "imgui_engine_stats", "Saving tracked collections to " KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);

			std::ofstream f(KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);
			if (!f) {
				kengine_assert_failed(r, "Failed to open '", KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE, "' for writing");
				return;
			}

			nlohmann::json file_json;

			for (const auto & collection : collections) {
				nlohmann::json collection_json;
				for (const auto comp : collection.components) {
					const auto & name = r.get<core::name>(comp);
					collection_json.push_back(name.name.c_str());
				}
				file_json.push_back(collection_json);
			}

			f << file_json;
		}

		std::vector<collection> load_tracked_collections() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "imgui_engine_stats", "Loading tracked collections from " KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);

			std::vector<collection> ret;

			std::ifstream f(KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);
			if (!f)
				return ret;

			const auto file_json = nlohmann::json::parse(f);
			for (const auto & collection_json : file_json) {
				collection collection;

				bool first = true;
				for (const auto & name_json : collection_json) {
					if (!first)
						collection.name += " + ";
					first = false;
					collection.name += name_json;

					bool found = false;
					for (const auto & [e, name, has] : r.view<core::name, meta::has>().each())
						if (name.name.c_str() == name_json) {
							collection.components.push_back(e);
							found = true;
							break;
						}

					if (!found) {
						kengine_logf(r, warning, "imgui_engine_stats", "Missing type entity for %s", std::string(name_json).c_str());
						collection.missing_components.push_back(name_json);
					}
				}

				ret.push_back(std::move(collection));
			}

			return ret;
		}

		void find_missing_component_entities(collection & collection) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, "imgui_engine_stats", "Looking for missing components for collection %s", collection.name.c_str());

			for (size_t i = 0; i < collection.missing_components.size(); ++i) {
				const auto & comp_name = collection.missing_components[i];
				for (const auto & [e, name] : r.view<core::name>().each())
					if (comp_name == name.name) {
						kengine_logf(r, log, "imgui_engine_stats", "Found type entity for %s", comp_name.c_str());
						collection.components.push_back(e);
						collection.missing_components.erase(collection.missing_components.begin() + i);
						--i;
						break;
					}
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(imgui_engine_stats)
}
