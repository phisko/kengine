#pragma once

#include "Entity.hpp"
#include "EntityManager.hpp"
#include "components/SelectedComponent.hpp"
#include "reflection/Reflectible.hpp"
#include "imgui.h"

putils_member_detector(c_str);
namespace kengine {
	namespace packets {
		using HasComponentFunc = bool(*)(const kengine::Entity &);
		using AddComponentFunc = void(*)(kengine::Entity &);
		using RemoveComponentFunc = void(*)(kengine::Entity &);
		using DisplayComponentFunc = void(*)(const kengine::Entity &);
		using EditComponentFunc = void(*)(kengine::Entity &);
		using MatchesFunc = bool(*)(const kengine::Entity &, const char *); // Returns whether any element in component matches a string

		struct RegisterComponentEditor {
			const char * name;
			HasComponentFunc has;
			AddComponentFunc add;
			RemoveComponentFunc remove;
			DisplayComponentFunc display;
			EditComponentFunc edit;
			MatchesFunc matches;
		};
	}

	template<typename Comp>
	void registerComponentEditor(kengine::EntityManager & em);

	template<typename ... Comps>
	void registerComponentEditors(kengine::EntityManager & em);

	namespace detail {
		static kengine::EntityManager * g_em = nullptr;

		template<typename Member>
		static void displayAttribute(const char * name, const Member & member) {
			if constexpr (putils::has_member_c_str<Member>::value) {
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
					ImGui::Text("%zu", member);
					ImGui::SameLine();
					if (ImGui::Button("Select"))
						g_em->getEntity(member).attach<kengine::SelectedComponent>();
				}
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

			if constexpr (putils::has_member_c_str<Member>::value) {
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
					for (auto & val : member)
						editAttribute(putils::string<64>("%d", i++), val);
					ImGui::TreePop();
				}
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
				ImGui::InputScalar(name, sizeof(Member) == 64 ? ImGuiDataType_U64 : ImGuiDataType_U32, &member);
				ImGui::SameLine();
				if (ImGui::Button("Select"))
					g_em->getEntity(member).attach<kengine::SelectedComponent>();
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

		template<typename Member>
		static bool matchAttribute(const Member & member, const char * str) {
			if constexpr (putils::has_member_c_str<Member>::value) {
				return strstr(member.c_str(), str);
			}
			// else if constexpr (std::is_same_v<Member, const char *>::value)
			// 	ImGui::LabelText(name, member);

			else if constexpr (putils::is_iterable<Member>::value) {
				for (const auto & val : member)
					if (matchAttribute(val, str))
						return true;
				return false;
			}

			else if constexpr (putils::has_member_get_attributes<Member>::value) {
				bool matches = false;
				putils::for_each_attribute(Member::get_attributes(), [&](const char * name, const auto attr) {
					matches |= matchAttribute(member.*attr, str);
				});
				return matches;
			}

			else if constexpr (std::is_same_v<Member, bool>) {
				return (strcmp(str, "false") == 0 || strcmp(str, "true") == 0) && putils::parse<bool>(str) == member;
			}
			else if constexpr (std::is_same_v<Member, int>) {
				return (str[0] == '-' || (str[0] >= '0' && str[0] <= '9')) && putils::parse<int>(str) == member;
			}
			else if constexpr (std::is_same_v<Member, unsigned int>) {
				return str[0] >= '0' && str[0] <= '9' && putils::parse<unsigned int>(str) == member;
			}
			else if constexpr (std::is_same_v<Member, float>) {
				return (str[0] == '-' || str[0] == '.' || (str[0] >= '0' && str[0] <= '9')) && putils::parse<float>(str) == member;
			}
			else if constexpr (std::is_same_v<Member, double>) {
				return (str[0] == '-' || str[0] == '.' || (str[0] >= '0' && str[0] <= '9')) && putils::parse<double>(str) == member;
			}

			return false;
		}

		template<typename Comp>
		static bool componentMatches(const kengine::Entity & e, const char * str) {
			if (strstr(Comp::get_class_name(), str))
				return true;

			bool matches = false;
			if constexpr (putils::has_member_get_attributes<Comp>::value) {
				auto & comp = e.get<Comp>();
				putils::for_each_attribute(Comp::get_attributes(), [&](const char * name, const auto member) {
					if (matchAttribute(comp.*member, str))
						matches = true;
				});
			}
			return matches;
		}
	}

	template<typename Comp>
	void registerComponentEditor(kengine::EntityManager & em) {
		detail::g_em = &em;

		em.send(packets::RegisterComponentEditor{
			Comp::get_class_name(),
			[](const kengine::Entity & e) { return e.has<Comp>(); },
			[](kengine::Entity & e) { e.attach<Comp>(); },
			[](kengine::Entity & e) { e.detach<Comp>(); },
			detail::displayComponent<Comp>,
			detail::editComponent<Comp>,
			detail::componentMatches<Comp>
		});
	}

	template<typename ... Comps>
	void registerComponentEditors(kengine::EntityManager & em) {
		pmeta_for_each(Comps, [&](auto type) {
			using Type = pmeta_wrapped(type);
			registerComponentEditor<Type>(em);
		});
	}
}
