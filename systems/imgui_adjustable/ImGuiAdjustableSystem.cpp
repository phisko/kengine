#include "ImGuiAdjustableSystem.hpp"
#include "kengine.hpp"

// stl
#include <fstream>

// magic_enum
#include <magic_enum.hpp>

// imgui
#include <imgui.h>

// putils
#include "vector.hpp"
#include "to_string.hpp"
#include "visit.hpp"
#include "IniFile.hpp"
#include "static_assert.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "data/NameComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"
#include "functions/OnTerminate.hpp"
#include "functions/OnEntityCreated.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

#ifndef KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH
# define KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH "."
#endif

#ifndef KENGINE_ADJUSTABLE_SAVE_FILE
# define KENGINE_ADJUSTABLE_SAVE_FILE "adjust.ini"
#endif

#ifndef KENGINE_ADJUSTABLE_SEPARATOR
# define KENGINE_ADJUSTABLE_SEPARATOR ';'
#endif

#ifndef KENGINE_MAX_ADJUSTABLES_SECTIONS
# define KENGINE_MAX_ADJUSTABLES_SECTIONS 8
#endif

#ifndef KENGINE_MAX_ADJUSTABLES
# define KENGINE_MAX_ADJUSTABLES 256
#endif

namespace kengine::imgui_adjustable {
	struct impl {
		using string = AdjustableComponent::string;
		static inline putils::IniFile loadedFile;
		using Sections = putils::vector<string, KENGINE_MAX_ADJUSTABLES_SECTIONS>;

		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;

			load();
			e += functions::OnTerminate{ save };
			e += functions::OnEntityCreated{ onEntityCreated };

			e += NameComponent{ "Adjustables" };
			auto & tool = e.attach<ImGuiToolComponent>();
			tool.enabled = true;

			e += functions::Execute{ [&](float deltaTime) noexcept {
				if (!tool.enabled)
					return;
				kengine_log(Verbose, "Execute", "ImGuiAdjustableSystem");
				drawImGui(tool.enabled);
			} };
		}

		static void drawImGui(bool & enabled) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (ImGui::Begin("Adjustables", &enabled)) {
				static char nameSearch[1024] = "";

				ImGui::Columns(2);
				if (ImGui::Button("Save", { -1.f, 0.f }))
					save();
				ImGui::NextColumn();
				if (ImGui::Button("Load", { -1.f, 0.f }))
					load();
				ImGui::Columns();

				ImGui::Separator();
				ImGui::InputText("Name", nameSearch, sizeof(nameSearch));
				ImGui::Separator();

				if (ImGui::BeginChild("##adjustables")) {
					const auto comps = getFilteredComps(nameSearch);

					Sections previousSubsections;
					string previousSection;
					bool hidden = false;
					for (const auto comp : comps) {
						const bool sectionMatches = comp->section.find(nameSearch) != std::string::npos;

						if (comp->section != previousSection) {
							const auto subs = split(comp->section, '/');
							const auto current = updateImGuiTree(hidden, subs, previousSubsections);
							previousSubsections = subs;
							if (current < previousSubsections.size())
								previousSubsections.erase(previousSubsections.begin() + current + 1, previousSubsections.end());
							previousSection = reconstitutePath(previousSubsections);
						}

						if (hidden)
							continue;

						for (auto & value : comp->values)
							if (sectionMatches || value.name.find(nameSearch) != std::string::npos)
								draw(value.name.c_str(), value);
					}
					for (size_t i = hidden ? 1 : 0; i < previousSubsections.size(); ++i)
						ImGui::TreePop();
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}

		static putils::vector<AdjustableComponent *, KENGINE_MAX_ADJUSTABLES> getFilteredComps(const char * nameSearch) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::vector<AdjustableComponent *, KENGINE_MAX_ADJUSTABLES> comps;

			for (const auto & [e, comp] : entities.with<AdjustableComponent>()) {
				if (comp.section.find(nameSearch) != std::string::npos) {
					comps.emplace_back(&comp);
					continue;
				}

				for (const auto & value : comp.values)
					if (value.name.find(nameSearch) != std::string::npos) {
						comps.emplace_back(&comp);
						continue;
					}
			}

			std::sort(comps.begin(), comps.end(), [](const auto lhs, const auto rhs) noexcept {
				return strcmp(lhs->section.c_str(), rhs->section.c_str()) < 0;
				});

			return comps;
		}

		static Sections split(const string & s, char delim) noexcept {
			KENGINE_PROFILING_SCOPE;

			Sections ret;

			size_t previous = 0;
			size_t next = 0;
			while (next < s.size()) {
				next = s.find(delim, previous);
                if (next == string::npos)
                    next = s.size();
				ret.emplace_back(s.substr(previous, next - previous));
				previous = next + 1;
			}

			return ret;
		}

		static string reconstitutePath(const Sections & subSections) noexcept {
			KENGINE_PROFILING_SCOPE;

			string ret;

			bool first = true;
			for (const auto & s : subSections) {
				if (!first)
					ret += '/';
				first = false;
				ret += s;
			}

			return ret;
		}

		static size_t updateImGuiTree(bool & hidden, const Sections & subs, const Sections & previousSubsections) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto current = previousSubsections.size() - 1;

			if (!previousSubsections.empty()) {
				while (current >= subs.size()) {
					if (!hidden)
						ImGui::TreePop();
					hidden = false;
					--current;
				}

				while (subs[current] != previousSubsections[current]) {
					if (current == 0) {
						if (!hidden)
							ImGui::TreePop();
						hidden = false;
						--current;
						break;
					}
					if (!hidden)
						ImGui::TreePop();
					hidden = false;
					--current;
				}
			}

			if (!hidden) {
				++current;
				while (current < subs.size()) {
					if (!ImGui::TreeNodeEx(subs[current].c_str())) {
						hidden = true;
						break;
					}
					++current;
				}
			}

			return current;
		}

		static void draw(const char * name, AdjustableComponent::Value & value) noexcept {
			KENGINE_PROFILING_SCOPE;

			ImGui::Columns(2);
			ImGui::Text(name);
			ImGui::NextColumn();

            switch (value.type) {
                case AdjustableComponent::Value::Type::Int: {
                    auto & s = value.intStorage;
                    if (value.getEnumNames != nullptr)
                        ImGui::Combo((string("##") + value.name).c_str(), s.ptr != nullptr ? s.ptr : &s.value, value.getEnumNames(), (int)value.enumCount);
                    else {
                        ImGui::PushItemWidth(-1.f);
                        auto val = s.ptr != nullptr ? *s.ptr : s.value;
                        if (ImGui::InputInt((string("##") + value.name).c_str(), &val, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
                            s.value = val;
                            if (s.ptr != nullptr)
                                *s.ptr = val;
                        }
                        ImGui::PopItemWidth();
                    }
                    break;
                }
                case AdjustableComponent::Value::Type::Float: {
                    auto & s = value.floatStorage;
                    ImGui::PushItemWidth(-1.f);
                    auto val = s.ptr != nullptr ? *s.ptr : s.value;
                    if (ImGui::InputFloat((string("##") + value.name).c_str(), &val, 0.f, 0.f, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                        s.value = val;
                        if (s.ptr != nullptr)
                            *s.ptr = val;
                    }
                    ImGui::PopItemWidth();
                    break;
                }
                case AdjustableComponent::Value::Type::Bool: {
                    auto & s = value.boolStorage;
                    ImGui::Checkbox((string("##") + value.name).c_str(), s.ptr != nullptr ? s.ptr : &s.value);
                    if (s.ptr != nullptr)
                        s.value = *s.ptr;
                    break;
                }
                case AdjustableComponent::Value::Type::Color: {
                    auto & s = value.colorStorage;
                    const auto color = s.ptr != nullptr ? s.ptr->attributes : s.value.attributes;
                    if (ImGui::ColorButton((string("##") + value.name).c_str(), ImVec4(color[0], color[1], color[2], color[3])))
                        ImGui::OpenPopup("color picker popup");

                    if (ImGui::BeginPopup("color picker popup")) {
                        ImGui::ColorPicker4(value.name, color);
                        ImGui::EndPopup();
                    }
                    if (s.ptr != nullptr)
                        s.value = *s.ptr;
                    break;
                }
                default: {
                    static_assert(magic_enum::enum_count<AdjustableComponent::Value::Type>() == 5); // + 1 for Invalid
                    kengine_assert_failed("Unknown AdjustableComponent::Value type");
                    break;
                }
            }

			ImGui::Columns();
		}

		static void onEntityCreated(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto adjustable = e.tryGet<AdjustableComponent>();
			if (!adjustable)
				return;

			initAdjustable(*adjustable);
		}

		static void initAdjustable(AdjustableComponent & comp) noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine_logf(Log, "Init/ImGuiAdjustableSystem", "Initializing section %s", comp.section.c_str());

			const auto it = loadedFile.sections.find(comp.section.c_str());
			if (it == loadedFile.sections.end()) {
				kengine_log(Warning, "Init/ImGuiAdjustableSystem", "Section not found in INI file");
				return;
			}
			const auto & section = it->second;
			for (auto & value : comp.values) {
				const auto it = section.values.find(value.name.c_str());
				if (it != section.values.end()) {
					kengine_logf(Log, "Init/ImGuiAdjustableSystem", "Initializing %s", value.name.c_str());
					setValue(value, it->second.c_str());
				}
				else
					kengine_logf(Log, "Init/ImGuiAdjustableSystem", "Value not found in INI for %s", value.name.c_str());
			}
		}

		static void setValue(AdjustableComponent::Value & value, const char * s) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto assignPtr = [](auto & storage) {
				if (storage.ptr != nullptr)
					*storage.ptr = storage.value;
			};

            switch (value.type) {
                case AdjustableComponent::Value::Type::Int: {
                    value.intStorage.value = putils::parse<int>(s);
                    assignPtr(value.intStorage);
                    break;
                }
                case AdjustableComponent::Value::Type::Float: {
                    value.floatStorage.value = putils::parse<float>(s);
                    assignPtr(value.floatStorage);
                    break;
                }
                case AdjustableComponent::Value::Type::Bool: {
                    value.boolStorage.value = putils::parse<bool>(s);
                    assignPtr(value.boolStorage);
                    break;
                }
                case AdjustableComponent::Value::Type::Color: {
                    putils::Color tmp;
                    tmp.rgba = putils::parse<unsigned int>(s);
                    value.colorStorage.value = putils::toNormalizedColor(tmp);
                    assignPtr(value.colorStorage);
                    break;
                }
                default: {
                    static_assert(magic_enum::enum_count<AdjustableComponent::Value::Type>() == 5); // + 1 for Invalid
                    kengine_assert_failed("Unknown AdjustableComponent::Value type");
                    break;
                }
            }
		}

		static void load() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "ImGuiAdjustableSystem", "Loading from " KENGINE_ADJUSTABLE_SAVE_FILE);

            std::ifstream f(KENGINE_ADJUSTABLE_SAVE_FILE);
            f >> loadedFile;
			for (auto [e, comp] : entities.with<AdjustableComponent>())
				initAdjustable(comp);
		}

		static void save() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "ImGuiAdjustableSystem", "Saving to " KENGINE_ADJUSTABLE_SAVE_FILE);

			std::ofstream f(KENGINE_ADJUSTABLE_SAVE_FILE, std::ofstream::trunc);
			if (!f) {
				kengine_assert_failed("Failed to open '", KENGINE_ADJUSTABLE_SAVE_FILE, "' with write permissions");
				return;
			}

			putils::IniFile ini;

			for (const auto & [e, comp] : entities.with<AdjustableComponent>()) {
				auto & section = ini.sections[comp.section.c_str()];

				for (const auto & value : comp.values) {
					auto & iniValue = section.values[value.name.c_str()];

                    switch (value.type) {
                        case AdjustableComponent::Value::Type::Int: {
                            iniValue = putils::toString(value.intStorage.value);
                            break;
                        }
                        case AdjustableComponent::Value::Type::Float: {
                            iniValue = putils::toString(value.floatStorage.value);
                            break;
                        }
                        case AdjustableComponent::Value::Type::Bool: {
                            iniValue = putils::toString(value.boolStorage.value);
                            break;
                        }
                        case AdjustableComponent::Value::Type::Color: {
                            iniValue = putils::toString(putils::toRGBA(value.colorStorage.value));
                            break;
                        }
                        default: {
                            static_assert(magic_enum::enum_count<AdjustableComponent::Value::Type>() == 5); // + 1 for Invalid
                            kengine_assert_failed("Unknown AdjustableComponent::Value type");
                            break;
                        }
                    }
				}
			}

			f << ini;
		}
	};
}

namespace kengine {
	EntityCreator * ImGuiAdjustableSystem() noexcept {
		KENGINE_PROFILING_SCOPE;
		return [](Entity & e) noexcept {
			imgui_adjustable::impl::init(e);
		};
	}
}
