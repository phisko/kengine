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

// putils
#include "forward_to.hpp"

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
		entt::registry & r;
		bool * enabled = nullptr;

		ImGuiEngineStatsSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "ImGuiEngineStatsSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));

			e.emplace<NameComponent>("Engine stats");
			auto & tool = e.emplace<ImGuiToolComponent>(true);
			enabled = &tool.enabled;
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*enabled)
				return;

			kengine_log(r, Verbose, "Execute", "ImGuiEngineStatsSystem");

			if (ImGui::Begin("Engine stats", enabled)) {
				ImGui::Text("Entities: %zu", r.alive());
				ImGui::Text("\tEntity pool size: %zu", r.size());
				const auto componentCount = std::ranges::count_if(r.storage(), [](auto &&) { return true; });
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
		std::vector<Collection> tracked = loadTrackedCollections();
		void displayTrackedCombinations() noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto newCollection = displayCombinationCreator();
			if (newCollection) {
				kengine_logf(r, Log, "ImGuiEngineStatsSystem", "New tracked collection: %s", newCollection->name.c_str());
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

		Collection creating;
		std::optional<Collection> displayCombinationCreator() noexcept {
			KENGINE_PROFILING_SCOPE;

			if (ImGui::Button("Track new collection", { -1.f, 0.f }))
				ImGui::OpenPopup("Create collection");

			if (ImGui::BeginPopup("Create collection")) {

				if (ImGui::Button("Track", { -1.f, 0.f })) {
					ImGui::CloseCurrentPopup();
					if (!creating.components.empty()) {
						Collection ret;
						std::swap(ret, creating);

						bool first = true;
						for (const auto id : ret.components) {
							if (!first)
								ret.name += " + ";
							ret.name += r.get<NameComponent>(id).name;
							first = false;
						}

						ImGui::EndPopup();
						return ret;
					}
				}

				const auto sortedEntities = sortHelper::getNameSortedEntities<meta::Has>(r);

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

		size_t getCollectionCount(const Collection & collection) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (collection.components.size() == 1) {
				const auto comp = collection.components[0];
				const auto count = r.try_get<meta::Count>(comp);
				if (count)
					return count->call();
			}

			size_t ret = 0;
			r.each([&](entt::entity e) {
				bool good = true;

				for (const auto comp : collection.components) {
					const auto & has = r.get<meta::Has>(comp);
					if (!has({ r, e })) {
						good = false;
						break;
					}
				}

				if (good)
					++ret;
			});

			return ret;
		}

		void saveTrackedCollections(const std::vector<Collection> & collections) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "ImGuiEngineStatsSystem", "Saving tracked collections to " KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);

			std::ofstream f(KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);
			if (!f) {
				kengine_assert_failed(r, "Failed to open '", KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE, "' for writing");
				return;
			}

			nlohmann::json fileJSON;

			for (const auto & collection : collections) {
				nlohmann::json collectionJSON;
				for (const auto comp : collection.components) {
					const auto & name = r.get<NameComponent>(comp);
					collectionJSON.push_back(name.name.c_str());
				}
				fileJSON.push_back(collectionJSON);
			}

			f << fileJSON;
		}

		std::vector<Collection> loadTrackedCollections() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "ImGuiEngineStatsSystem", "Loading tracked collections from " KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);

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
					for (const auto & [e, name, has] : r.view<NameComponent, meta::Has>().each())
						if (name.name.c_str() == nameJSON) {
							collection.components.push_back(e);
							found = true;
							break;
						}

					if (!found) {
						kengine_logf(r, Log, "ImGuiEngineStatsSystem/loadTrackedCollections", "Missing component Entity for %s", std::string(nameJSON).c_str());
						collection.missingComponents.push_back(nameJSON);
					}
				}

				ret.push_back(std::move(collection));
			}

			return ret;
		}

		void findMissingComponentEntities(Collection & collection) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (size_t i = 0; i < collection.missingComponents.size(); ++i) {
				const auto & compName = collection.missingComponents[i];
				for (const auto & [e, name] : r.view<NameComponent>().each())
					if (compName == name.name) {
						kengine_logf(r, Log, "ImGuiEngineStatsSystem", "Found component Entity for %s", compName.c_str());
						collection.components.push_back(e);
						collection.missingComponents.erase(collection.missingComponents.begin() + i);
						--i;
						break;
					}
			}
		}
	};

	void addImGuiEngineStatsSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<ImGuiEngineStatsSystem>(e);
	}
}
