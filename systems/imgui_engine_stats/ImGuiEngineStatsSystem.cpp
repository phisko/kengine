#include "ImGuiEngineStatsSystem.hpp"

// stl
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// nlohmann
#include <nlohmann/json.hpp>

// imgui
#include <imgui.h>

// kengine data
#include "data/NameComponent.hpp"
#include "data/ImGuiToolComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine meta
#include "meta/Count.hpp"
#include "meta/Has.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"
#include "helpers/sortHelper.hpp"

#ifndef KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE
# define KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE "trackedEntityCollections.json"
#endif

namespace kengine {
	struct ImGuiEngineStatsSystem {
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "ImGuiEngineStatsSystem");

			_r = &r;

			const auto e = r.create();
			r.emplace<functions::Execute>(e, execute);

			r.emplace<NameComponent>(e, "Engine stats");
			auto & tool = r.emplace<ImGuiToolComponent>(e, true);
			_enabled = &tool.enabled;
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*_enabled)
				return;

			kengine_log(*_r, Verbose, "Execute", "ImGuiEngineStatsSystem");

			if (ImGui::Begin("Engine stats", _enabled)) {
				ImGui::Text("Entities: %zu", _r->alive());
				ImGui::Text("\tEntity pool size: %zu", _r->size());
				const auto componentCount = std::ranges::count_if(_r->storage(), [](auto &&) { return true; });
				ImGui::Text("Component types: %zu", componentCount);
				displayTrackedCombinations();
			}
			ImGui::End();
		}

		struct Collection {
			std::string name;
			std::vector<entt::entity> components;
			std::vector<std::string> missingComponents;
		};

		static void displayTrackedCombinations() noexcept {
			KENGINE_PROFILING_SCOPE;

			static std::vector<Collection> tracked = loadTrackedCollections();

			const auto newCollection = displayCombinationCreator();
			if (newCollection) {
				kengine_logf(*_r, Log, "ImGuiEngineStatsSystem", "New tracked collection: %s", newCollection->name.c_str());
				tracked.push_back(*newCollection);
				saveTrackedCollections(tracked);
			}

			for (size_t i = 0; i < tracked.size(); ++i) {
				auto & collection = tracked[i];
				if (collection.missingComponents.empty()) {
					const auto count = getCollectionCount(collection);
					ImGui::Text("%s: %zu", collection.name.c_str(), count);
				}
				else {
					std::string missingComponents;

					bool first = true;
					for (const auto & s : collection.missingComponents) {
						if (first)
							missingComponents += ", ";
						first = false;
						missingComponents += s;
					}

					ImGui::Text("%s: missing Component Entities for %s", collection.name.c_str(), missingComponents.c_str());
					findMissingComponentEntities(collection);
				}

				if (ImGui::BeginPopupContextItem("Remove collection")) {
					if (ImGui::MenuItem("Remove")) {
						tracked.erase(tracked.begin() + i);
						--i;
					}
					ImGui::EndPopup();
				}
			}
		}

		static std::optional<Collection> displayCombinationCreator() noexcept {
			KENGINE_PROFILING_SCOPE;

			if (ImGui::Button("Track new collection", { -1.f, 0.f }))
				ImGui::OpenPopup("Create collection");

			if (ImGui::BeginPopup("Create collection")) {
				static Collection creating;

				if (ImGui::Button("Track", { -1.f, 0.f })) {
					ImGui::CloseCurrentPopup();
					if (!creating.components.empty()) {
						Collection ret;
						std::swap(ret, creating);

						bool first = true;
						for (const auto id : ret.components) {
							if (!first)
								ret.name += " + ";
							ret.name += _r->get<NameComponent>(id).name;
							first = false;
						}

						ImGui::EndPopup();
						return ret;
					}
				}

				const auto sortedEntities = sortHelper::getNameSortedEntities<meta::Has>(*_r);

				for (const auto [e, name, has] : sortedEntities) {
					const auto it = std::ranges::find(creating.components, e);
					bool inCreating = it != creating.components.end();
					if (ImGui::Checkbox(name->name.c_str(), &inCreating)) {
						if (inCreating)
							creating.components.push_back(e);
						else
							creating.components.erase(it);
					}
				}
				ImGui::EndPopup();
			}

			return std::nullopt;
		}

		static size_t getCollectionCount(const Collection & collection) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (collection.components.size() == 1) {
				const auto comp = collection.components[0];
				const auto count = _r->try_get<meta::Count>(comp);
				if (count)
					return count->call();
			}

			size_t ret = 0;
			_r->each([&](entt::entity e) {
				bool good = true;

				for (const auto comp : collection.components) {
					const auto & has = _r->get<meta::Has>(comp);
					if (!has({ *_r, e })) {
						good = false;
						break;
					}
				}

				if (good)
					++ret;
			});

			return ret;
		}

		static void saveTrackedCollections(const std::vector<Collection> & collections) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Log, "ImGuiEngineStatsSystem", "Saving tracked collections to " KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);

			std::ofstream f(KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);
			if (!f) {
				kengine_assert_failed(*_r, "Failed to open '", KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE, "' for writing");
				return;
			}

			nlohmann::json fileJSON;

			for (const auto & collection : collections) {
				nlohmann::json collectionJSON;
				for (const auto comp : collection.components) {
					const auto & name = _r->get<NameComponent>(comp);
					collectionJSON.push_back(name.name.c_str());
				}
				fileJSON.push_back(collectionJSON);
			}

			f << fileJSON;
		}

		static std::vector<Collection> loadTrackedCollections() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Log, "ImGuiEngineStatsSystem", "Loading tracked collections from " KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);

			std::vector<Collection> ret;

			std::ifstream f(KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);
			if (!f)
				return ret;

			const auto fileJSON = nlohmann::json::parse(f);
			for (const auto & collectionJSON : fileJSON) {
				Collection collection;

				bool first = true;
				for (const auto & nameJSON : collectionJSON) {
					if (!first)
						collection.name += " + ";
					first = false;
					collection.name += nameJSON;

					bool found = false;
					for (const auto & [e, name, has] : _r->view<NameComponent, meta::Has>().each())
						if (name.name.c_str() == nameJSON) {
							collection.components.push_back(e);
							found = true;
							break;
						}

					if (!found) {
						kengine_logf(*_r, Log, "ImGuiEngineStatsSystem/loadTrackedCollections", "Missing component Entity for %s", std::string(nameJSON).c_str());
						collection.missingComponents.push_back(nameJSON);
					}
				}

				ret.push_back(std::move(collection));
			}

			return ret;
		}

		static void findMissingComponentEntities(Collection & collection) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (size_t i = 0; i < collection.missingComponents.size(); ++i) {
				const auto & compName = collection.missingComponents[i];
				for (const auto & [e, name] : _r->view<NameComponent>().each())
					if (compName == name.name) {
						kengine_logf(*_r, Log, "ImGuiEngineStatsSystem", "Found component Entity for %s", compName.c_str());
						collection.components.push_back(e);
						collection.missingComponents.erase(collection.missingComponents.begin() + i);
						--i;
						break;
					}
			}
		}

		static inline bool * _enabled;
		static inline entt::registry * _r;
	};

	void ImGuiEngineStatsSystem(entt::registry & r) noexcept {
		ImGuiEngineStatsSystem::init(r);
	}
}
