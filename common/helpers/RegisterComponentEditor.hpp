#pragma once

#include "EntityManager.hpp"
#include "Color.hpp"
#include "components/SelectedComponent.hpp"
#include "reflection.hpp"
#include "functions/ImGuiEditor.hpp"
#include "imgui.h"
#include "magic_enum.hpp"
#include "lengthof.hpp"

namespace kengine {
	template<typename Comp>
	void registerComponentEditor(kengine::EntityManager & em);

	template<typename ... Comps>
	void registerComponentEditors(kengine::EntityManager & em);
}

namespace kengine {
	namespace detail {
		namespace imguiEditor {
			putils_member_detector(c_str);
			putils_member_detector(emplace_back);
			inline kengine::EntityManager * g_em = nullptr;
		}

		template<typename F>
		static void displayInColumns(const char * name, F && f) {
			ImGui::Columns(2);
			ImGui::Text(name);
			ImGui::NextColumn();
			f();
			ImGui::Columns();
		}

		template<typename Member>
		static void displayAttribute(const char * name, const Member & member) {
			if constexpr (imguiEditor::has_member_c_str<Member>::value) {
				displayInColumns(name, [&] {
					ImGui::Text(member.c_str());
				});
			}
			// else if constexpr (std::is_same_v<Member, const char *>::value)
			// 	ImGui::LabelText(name, member);

			else if constexpr (putils::is_iterable<Member>::value) {
				if (ImGui::TreeNode(name)) {
					int i = 0;
					for (const auto & val : member)
						displayAttribute(putils::string<64>("%d", i++), val);
					ImGui::TreePop();
				}
			}

			else if constexpr (std::is_same_v<Member, putils::Color>) {
				displayInColumns(name, [&] {
					const auto normalized = putils::toNormalizedColor(member);
					const ImVec4 col = { normalized.r, normalized.g, normalized.b, normalized.a };
					ImGui::ColorButton(putils::string<64>(name) + "#" + (intptr_t)& member, col);
				});
			}
			else if constexpr (std::is_same_v<Member, putils::NormalizedColor>) {
				displayInColumns(name, [&] {
					const ImVec4 col = { member.r, member.g, member.b, member.a };
					ImGui::ColorButton(putils::string<64>(name) + "#" + (intptr_t)& member, col);
				});
			}

			else if constexpr (putils::has_member_get_attributes<Member>::value) {
				if (ImGui::TreeNode(name)) {
					putils::for_each_attribute<Member>([&member](const char * name, const auto attr) {
						displayAttribute(name, member.*attr);
					});
					ImGui::TreePop();
				}
			}
			else {
				displayInColumns(name, [&] {
					if constexpr (std::is_same_v<Member, kengine::Entity::ID>) {
						if (ImGui::Button("Select"))
							imguiEditor::g_em->getEntity(member).attach<kengine::SelectedComponent>();
						ImGui::SameLine();
						ImGui::Text("%zu", member);
					}
					else if constexpr (std::is_enum_v<Member>)
						ImGui::Text(putils::magic_enum::enum_name(member).data());
					else if constexpr (std::is_same_v<Member, bool>)
						ImGui::Text(member ? "true" : "false");
					else if constexpr (std::is_same_v<Member, int>)
						ImGui::Text("%d", member);
					else if constexpr (std::is_same_v<Member, unsigned int>)
						ImGui::Text("%zu", member);
					else if constexpr (std::is_same_v<Member, float>)
						ImGui::Text("%f", member);
					else if constexpr (std::is_same_v<Member, double>)
						ImGui::Text("%d", member);
				});
			}
		}

		template<typename Comp>
		static void displayComponent(const kengine::Entity & e) {
			if constexpr (putils::has_member_get_attributes<Comp>::value) {
				const auto & comp = e.get<Comp>();
				putils::for_each_attribute<Comp>([&comp](const char * name, const auto member) {
					displayAttribute(name, comp.*member);
				});
			}
		}

		template<typename Member>
		putils::string<64> getID(const char * name, Member && member) {
			return putils::string<64>("##%s", name) + (intptr_t)&member;
		}

		template<typename MemberRef>
		static void editAttribute(const char * name, MemberRef && member) {
			using Member = std::remove_reference_t<MemberRef>;

			if constexpr (imguiEditor::has_member_c_str<Member>::value) {
				displayInColumns(name, [&] {
					putils::string<1024> s = member.c_str();
					ImGui::PushItemWidth(-1.f);
					if (ImGui::InputText(getID(name, member), s.begin(), s.max_size, ImGuiInputTextFlags_EnterReturnsTrue))
						member = s.c_str();
					ImGui::PopItemWidth();
				});
			}
			// else if constexpr (std::is_same_v<Member, const char *>::value)
			// 	ImGui::LabelText(name, member);

			else if constexpr (putils::is_iterable<Member>::value) {
				if (ImGui::TreeNode(name)) {
					if constexpr (imguiEditor::has_member_emplace_back<Member>::value)
						if (ImGui::Button("Add"))
							member.emplace_back();
					int i = 0;
					for (auto & val : member)
						editAttribute(putils::string<64>("%d", i++), val);
					ImGui::TreePop();
				}
			}

			else if constexpr (std::is_same_v<Member, putils::Color>) {
				displayInColumns(name, [&] {
					auto normalized = putils::toNormalizedColor(member);
					const ImVec4 col = { normalized.r, normalized.g, normalized.b, normalized.a };

					if (ImGui::ColorButton(getID(name, member), col))
						ImGui::OpenPopup(name);

					if (ImGui::BeginPopup(name)) {
						if (ImGui::ColorPicker4(name, normalized.attributes))
							member = putils::toColor(normalized);
						ImGui::EndPopup();
					}
				});
			}
			else if constexpr (std::is_same_v<Member, putils::NormalizedColor>) {
				displayInColumns(name, [&] {
					const ImVec4 col = { member.r, member.g, member.b, member.a };
					if (ImGui::ColorButton(getID(name, member), col))
						ImGui::OpenPopup(name);

					if (ImGui::BeginPopup(name)) {
						ImGui::ColorPicker4(name, member.attributes);
						ImGui::EndPopup();
					}
				});
			}

			else if constexpr (putils::has_member_get_attributes<Member>::value) {
				if (ImGui::TreeNode(name)) {
					putils::for_each_attribute<Member>([&member](const char * name, const auto attr) {
						editAttribute(name, member.*attr);
					});
					ImGui::TreePop();
				}
			}

			else if constexpr (std::is_same_v<Member, kengine::Entity::ID>) {
				displayInColumns(name, [&] {
					if (ImGui::Button("Select"))
						imguiEditor::g_em->getEntity(member).attach<kengine::SelectedComponent>();
					ImGui::SameLine();
					ImGui::PushItemWidth(-1.f);
					auto val = member;
					if (ImGui::InputScalar(getID(name, member), sizeof(Member) == 64 ? ImGuiDataType_U64 : ImGuiDataType_U32, &val, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue))
						member = val;
					ImGui::PopItemWidth();
				});
			}
			else if constexpr (std::is_enum_v<Member>) {
				static_assert(std::is_same_v<std::underlying_type_t<Member>, int>);

				displayInColumns(name, [&] {
					static putils::string<64> names[putils::magic_enum::enum_count<Member>()];
					static bool first = true;
					if (first) {
						for (int i = 0; i < lengthof(names); ++i)
							names[i] = putils::magic_enum::enum_names<Member>()[i];
						first = false;
					}
					ImGui::Combo(getID(name, member), (int *)& member, [](void *, int idx, const char ** out) { *out = names[idx].c_str(); return true; }, nullptr, lengthof(names));
				});
			}
			else if constexpr (std::is_same_v<Member, bool>) {
				displayInColumns(name, [&] {
					ImGui::Checkbox(getID(name, member), &member);
				});
			}
			else if constexpr (std::is_same_v<Member, int>) {
				displayInColumns(name, [&] {
					ImGui::PushItemWidth(-1.f);
					auto val = member;
					if (ImGui::InputInt(getID(name, member), &val, ImGuiInputTextFlags_EnterReturnsTrue))
						member = val;
					ImGui::PopItemWidth();
				});
			}
			else if constexpr (std::is_same_v<Member, unsigned int>) {
				displayInColumns(name, [&] {
					ImGui::PushItemWidth(-1.f);
					auto val = member;
					if (ImGui::InputScalar(getID(name, member), sizeof(Member) == 64 ? ImGuiDataType_U64 : ImGuiDataType_U32, &val, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue))
						member = val;
					ImGui::PopItemWidth();
				});
			}
			else if constexpr (std::is_same_v<Member, float>) {
				displayInColumns(name, [&] {
					ImGui::PushItemWidth(-1.f);
					auto val = member;
					if (ImGui::InputFloat(getID(name, member), &val, 0.f, 0.f, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue))
						member = val;
					ImGui::PopItemWidth();
				});
			}
			else if constexpr (std::is_same_v<Member, double>) {
				displayInColumns(name, [&] {
					ImGui::PushItemWidth(-1.f);
					auto val = member;
					if (ImGui::InputDouble(getID(name, member), &val, 0.0, 0.0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue))
						member = val;
					ImGui::PopItemWidth();
				});
			}
			else {
				displayInColumns(name, [&] {
					ImGui::Text("Unknown type");
				});
			}
		}

		template<typename Comp>
		static void editComponent(kengine::Entity & e) {
			if constexpr (putils::has_member_get_attributes<Comp>::value) {
				auto & comp = e.get<Comp>();
				putils::for_each_attribute<Comp>([&comp](const char * name, const auto member) {
					editAttribute(name, comp.*member);
				});
			}
		}
	}

	template<typename Comp>
	void registerComponentEditor(kengine::EntityManager & em) {
		detail::imguiEditor::g_em = &em;
		em.registerComponentFunction<Comp>(functions::DisplayImGui{ detail::displayComponent<Comp> });
		em.registerComponentFunction<Comp>(functions::EditImGui{ detail::editComponent<Comp> });
	}

	template<typename ... Comps>
	void registerComponentEditors(kengine::EntityManager & em) {
		pmeta_for_each(Comps, [&](auto type) {
			using Type = pmeta_wrapped(type);
			registerComponentEditor<Type>(em);
		});
	}
}