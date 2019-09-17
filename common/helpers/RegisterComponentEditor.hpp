#pragma once

#include "EntityManager.hpp"
#include "Color.hpp"
#include "components/SelectedComponent.hpp"
#include "reflection/Reflectible.hpp"
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

		template<typename Member>
		static void displayAttribute(const char * name, const Member & member) {
			if constexpr (imguiEditor::has_member_c_str<Member>::value) {
				ImGui::Columns(2);
				ImGui::Text(name);
				ImGui::NextColumn();
				ImGui::Text(member.c_str());
				ImGui::Columns();
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
				const auto normalized = putils::toNormalizedColor(member);
				const ImVec4 col = { normalized.r, normalized.g, normalized.b, normalized.a };
				ImGui::ColorButton(putils::string<64>(name) + "#" + (intptr_t)&member, col);
				ImGui::SameLine();
				ImGui::Text(name);
			}
			else if constexpr (std::is_same_v<Member, putils::NormalizedColor>) {
				const ImVec4 col = { member.r, member.g, member.b, member.a };
				ImGui::ColorButton(putils::string<64>(name) + "#" + (intptr_t)&member, col);
				ImGui::SameLine();
				ImGui::Text(name);
			}

			else if constexpr (putils::has_member_get_attributes<Member>::value) {
				if (ImGui::TreeNode(name)) {
					putils::for_each_attribute(Member::get_attributes(), [&member](const char * name, const auto attr) {
						displayAttribute(name, member.*attr);
					});
					ImGui::TreePop();
				}
			}
			else {
				ImGui::Columns(2);
				ImGui::Text(name);
				ImGui::NextColumn();
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
				ImGui::Columns();
			}
		}

		template<typename Comp>
		static void displayComponent(const kengine::Entity & e) {
			if constexpr (putils::has_member_get_attributes<Comp>::value) {
				const auto & comp = e.get<Comp>();
				putils::for_each_attribute(Comp::get_attributes(), [&comp](const char * name, const auto member) {
					displayAttribute(name, comp.*member);
				});
			}
		}

		template<typename MemberRef>
		static void editAttribute(const char * name, MemberRef && member) {
			using Member = std::decay_t<MemberRef>;

			if constexpr (imguiEditor::has_member_c_str<Member>::value) {
				putils::string<1024> s = member.c_str();
				if (ImGui::InputText(name, s.begin(), s.max_size))
					member = s.c_str();
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
				auto normalized = putils::toNormalizedColor(member);
				const ImVec4 col = { normalized.r, normalized.g, normalized.b, normalized.a };

				if (ImGui::ColorButton(putils::string<64>(name) + "#" + (intptr_t)&member, col))
					ImGui::OpenPopup("color picker popup");

				if (ImGui::BeginPopup("#color picker popup")) {
					if (ImGui::ColorPicker4(name, normalized.attributes))
						member = putils::toColor(normalized);
					ImGui::EndPopup();
				}
				ImGui::Text(name);
			}
			else if constexpr (std::is_same_v<Member, putils::NormalizedColor>) {
				const ImVec4 col = { member.r, member.g, member.b, member.a };
				if (ImGui::ColorButton(putils::string<64>(name) + "#" + (intptr_t)&member, col))
					ImGui::OpenPopup("color picker popup");

				if (ImGui::BeginPopup("color picker popup")) {
					ImGui::ColorPicker4(name, member.attributes);
					ImGui::EndPopup();
				}
				ImGui::SameLine();
				ImGui::Text(name);
			}

			else if constexpr (putils::has_member_get_attributes<Member>::value) {
				if (ImGui::TreeNode(name)) {
					putils::for_each_attribute(Member::get_attributes(), [&member](const char * name, const auto attr) {
						editAttribute(name, member.*attr);
					});
					ImGui::TreePop();
				}
			}

			else if constexpr (std::is_same_v<Member, kengine::Entity::ID>) {
				if (ImGui::Button("Select"))
					imguiEditor::g_em->getEntity(member).attach<kengine::SelectedComponent>();
				ImGui::SameLine();
				ImGui::InputScalar(name, sizeof(Member) == 64 ? ImGuiDataType_U64 : ImGuiDataType_U32, &member);
			}
			else if constexpr (std::is_enum_v<Member>) {
				static putils::string<64> names[putils::magic_enum::enum_count<Member>()];
				static bool first = true;
				if (first) {
					for (int i = 0; i < lengthof(names); ++i)
						names[i] = putils::magic_enum::enum_names<Member>()[i];
					first = false;
				}
				ImGui::Combo(name, (int*)&member, [](void *, int idx, const char ** out) { *out = names[idx].c_str(); return true; }, nullptr, lengthof(names));
			}
			else if constexpr (std::is_same_v<Member, bool>)
				ImGui::Checkbox(name, &member);
			else if constexpr (std::is_same_v<Member, int>)
				ImGui::InputInt(name, &member);
			else if constexpr (std::is_same_v<Member, unsigned int>)
				ImGui::InputScalar(name, sizeof(Member) == 64 ? ImGuiDataType_U64 : ImGuiDataType_U32, &member);
			else if constexpr (std::is_same_v<Member, float>)
				ImGui::InputFloat(name, &member);
			else if constexpr (std::is_same_v<Member, double>)
				ImGui::InputDouble(name, &member);
			else
				ImGui::Text(name);
		}

		template<typename Comp>
		static void editComponent(kengine::Entity & e) {
			if constexpr (putils::has_member_get_attributes<Comp>::value) {
				auto & comp = e.get<Comp>();
				putils::for_each_attribute(Comp::get_attributes(), [&comp](const char * name, const auto member) {
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