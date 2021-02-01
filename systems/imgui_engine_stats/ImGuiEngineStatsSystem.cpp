#include "ImGuiEngineStatsSystem.hpp"

#include <fstream>

#include "kengine.hpp"
#include "impl/GlobalState.hpp"

#include "data/NameComponent.hpp"
#include "data/ImGuiToolComponent.hpp"

#include "functions/Execute.hpp"

#include "meta/Count.hpp"
#include "meta/Has.hpp"

#include "helpers/sortHelper.hpp"

#include "json.hpp"
#include "imgui.h"

#ifndef KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE
# define KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE "trackedEntityCollections.json"
#endif

namespace kengine {
	EntityCreator * ImGuiEngineStatsSystem() noexcept {
		static bool * enabled;

		struct impl {
			static void init(Entity & e) noexcept {
				e += NameComponent{ "Engine stats" };
				auto & tool = e.attach<ImGuiToolComponent>();
				tool.enabled = true;
				enabled = &tool.enabled;

				e += functions::Execute{ execute };
			}

			static void execute(float deltaTime) noexcept {
				if (!*enabled)
					return;

				if (ImGui::Begin("Engine stats")) {
					const auto entityCount = std::count_if(
						kengine::impl::state->_entities.begin(), kengine::impl::state->_entities.end(),
						[](const kengine::impl::GlobalState::EntityMetadata & e) {
							return e.active && e.mask != 0;
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
				static std::vector<Collection> tracked = loadTrackedCollections();

				const auto newCollection = displayCombinationCreator();
				if (newCollection) {
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
				for (const auto & collection : tracked) {
				}
			}

			static std::optional<Collection> displayCombinationCreator() noexcept {
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
						if (ImGui::Checkbox(name->name, &inCreating)) {
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
				std::ofstream f(KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);
				if (!f) {
					kengine_assert_failed("Failed to open '", KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE, "' for writing");
					return;
				}

				putils::json fileJSON;

				for (const auto & collection : collections) {
					putils::json collectionJSON;
					for (const auto id : collection.components) {
						const auto & name = entities[id].get<NameComponent>();
						collectionJSON.push_back(name.name.c_str());
					}
					fileJSON.push_back(collectionJSON);
				}

				f << fileJSON;
			}

			static std::vector<Collection> loadTrackedCollections() noexcept {
				std::vector<Collection> ret;

				std::ifstream f(KENGINE_STATS_TRACKED_COLLECTIONS_SAVE_FILE);
				if (!f)
					return ret;

				const auto fileJSON = putils::json::parse(f);
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

						if (!found)
							collection.missingComponents.push_back(nameJSON);
					}

					ret.push_back(std::move(collection));
				}

				return ret;
			}

			static void findMissingComponentEntities(Collection & collection) noexcept {
				for (size_t i = 0; i < collection.missingComponents.size(); ++i) {
					const auto & compName = collection.missingComponents[i];
					for (const auto & [e, name] : entities.with<NameComponent>())
						if (compName == name.name) {
							collection.components.push_back(e.id);
							collection.missingComponents.erase(collection.missingComponents.begin() + i);
							--i;
							break;
						}
				}
			}
		};

		return impl::init;
	}
}
