#include "register_match_string.hpp"

// entt
#include <entt/entity/handle.hpp>

// sol
#ifdef KENGINE_LUA
#include <sol/sol.hpp>
#endif

// putils
#include "putils/to_string.hpp"
#include "putils/reflection_helpers/imgui_helper.hpp"

// kengine meta
#include "kengine/meta/match_string.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	namespace impl {
		template<typename Member>
		static bool match_attribute(const Member & member, const char * str) noexcept {
			KENGINE_PROFILING_SCOPE;

			if constexpr (requires { member.c_str(); }) {
				return strstr(member.c_str(), str);
			}
			// else if constexpr (std::is_same_v<Member, const char *>::value)
			// 	ImGui::LabelText(name, member);

			else if constexpr (std::ranges::range<Member>
#ifdef KENGINE_LUA
							   && !std::is_same<sol::object, Member>() && !std::is_base_of<sol::object, Member>()
#endif
			) {
				for (const auto & val : member)
					if (match_attribute(val, str))
						return true;
				return false;
			}

			else if constexpr (putils::reflection::has_attributes<Member>() || putils::reflection::has_parents<Member>()) {
				bool matches = false;
				putils::reflection::for_each_attribute(member, [&](const auto & attr) noexcept {
					matches |= match_attribute(attr.member, str);
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
	}

	template<typename... Comps>
	void register_match_string(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		register_meta_component_implementation<meta::match_string, Comps...>(
			r, [](const auto t, entt::const_handle e, const char * str) noexcept {
				KENGINE_PROFILING_SCOPE;

				using type = putils_wrapped_type(t);

				if constexpr (std::is_empty<type>())
					return false;
				else {
					const auto comp = e.try_get<type>();
					if (!comp)
						return false;

					if (strstr(putils::reflection::get_class_name<type>(), str))
						return true;

					bool matches = false;
					putils::reflection::for_each_attribute(*comp, [&](const auto & attr) noexcept {
						if (impl::match_attribute(attr.member, str))
							matches = true;
					});
					return matches;
				}
			}
		);
	}
}