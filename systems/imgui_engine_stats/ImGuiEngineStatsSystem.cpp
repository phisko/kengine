#include "ImGuiEngineStatsSystem.hpp"
#include "kengine.hpp"

// stl
#include <fstream>

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
#include "helpers/sortHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

// kengine impl
#include "impl/GlobalState.hpp"

#ifndef KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE
# define KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE "trackedEntityCollections.json"
#endif

namespace kengine {
	struct ImGuiEngineStatsSystem {
		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "ImGuiEngineStatsSystem");

			e += NameComponent{ "Engine stats" };
			auto & tool = e.attach<ImGuiToolComponent>();
			tool.enabled = true;
			_enabled = &tool.enabled;

			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*_enabled)
				return;

			kengine_log(Verbose, "Execute", "ImGuiEngineStatsSystem");

			if (ImGui::Begin("Engine stats", _enabled)) {
				const auto entityCount = std::count_if(
					kengine::impl::state->_entities.begin(), kengine::impl::state->_entities.end(),
					[](const kengine::impl::GlobalState::EntityMetadata & e) {
						return e.data.active && e.data.mask != 0;
					}
				);

				ImGui::Text("Entities: %zu", entityCount);
				ImGui::Text("\tEntity pool size: %zu", kengine::impl::state->_entities.size());
				ImGui::Text("Archetypes: %zu", kengine::impl::state->_archetypes.size());
				ImGui::Text("Component types: %zu", kengine::impl::state->_components.size());
				displayTrackedCombinations();
			}
			ImGui::End();
		}

		struct Collection {
			std::string name;
			std::vector<EntityID> components;
			std::vector<std::string> missingComponents;
		};

		static void displayTrackedCombinations() noexcept {
			KENGINE_PROFILING_SCOPE;

			static std::vector<Collection> tracked = loadTrackedCollections();

			const auto newCollection = displayCombinationCreator();
			if (newCollection) {
				kengine_logf(Log, "ImGuiEngineStatsSystem", "New tracked collection: %s", newCollection->name.c_str());
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
							ret.name += entities[id].get<NameComponent>().name;
							first = false;
						}

						ImGui::EndPopup();
						return ret;
					}
				}

				const auto sortedEntities = sortHelper::getNameSortedEntities<meta::Has>();

				for (const auto [e, name, has] : sortedEntities) {
					const auto it = std::find(creating.components.begin(), creating.components.end(), e.id);
					bool inCreating = it != creating.components.end();
					if (ImGui::Checkbox(name->name.c_str(), &inCreating)) {
						if (inCreating)
							creating.components.push_back(e.id);
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
				const auto comp = entities[collection.components[0]];
				const auto count = comp.tryGet<meta::Count>();
				if (count)
					return count->call();
			}

			size_t ret = 0;
			for (const auto & e : entities) {
				bool good = true;

				for (const auto id : collection.components) {
					const auto comp = entities[id];
					const auto & has = comp.get<meta::Has>();
					if (!has(e)) {
						good = false;
						break;
					}
				}

				if (good)
					++ret;
			}

			return ret;
		}

		static void saveTrackedCollections(const std::vector<Collection> & collections) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "ImGuiEngineStatsSystem", "Saving tracked collections to " KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);

			std::ofstream f(KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);
			if (!f) {
				kengine_assert_failed("Failed to open '", KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE, "' for writing");
				return;
			}

			nlohmann::json fileJSON;

			for (const auto & collection : collections) {
				nlohmann::json collectionJSON;
				for (const auto id : collection.components) {
					const auto & name = entities[id].get<NameComponent>();
					collectionJSON.push_back(name.name.c_str());
				}
				fileJSON.push_back(collectionJSON);
			}

			f << fileJSON;
		}

		static std::vector<Collection> loadTrackedCollections() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "ImGuiEngineStatsSystem", "Loading tracked collections from " KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);

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
					for (const auto & [e, name, has] : entities.with<NameComponent, meta::Has>())
						if (name.name.c_str() == nameJSON) {
							collection.components.push_back(e.id);
							found = true;
							break;
						}

					if (!found) {
						kengine_logf(Log, "ImGuiEngineStatsSystem/loadTrackedCollections", "Missing component Entity for %s", std::string(nameJSON).c_str());
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
				for (const auto & [e, name] : entities.with<NameComponent>())
					if (compName == name.name) {
						kengine_logf(Log, "ImGuiEngineStatsSystem", "Found component Entity for %s", compName.c_str());
						collection.components.push_back(e.id);
						collection.missingComponents.erase(collection.missingComponents.begin() + i);
						--i;
						break;
					}
			}
		}

		static inline bool * _enabled;
	};

	EntityCreator * ImGuiEngineStatsSystem() noexcept {
		return ImGuiEngineStatsSystem::init;
	}
}
