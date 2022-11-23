#include "registerMatchString.hpp"

// entt
#include <entt/entity/handle.hpp>

// sol
#ifdef KENGINE_LUA
# include <sol/sol.hpp>
#endif

// putils
#include "to_string.hpp"
#include "reflection_helpers/imgui_helper.hpp"

// kengine meta
#include "meta/MatchString.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	namespace impl {
		namespace matcher {
			putils_member_detector(c_str);
		}

		template<typename Member>
		static bool matchAttribute(const Member & member, const char * str) noexcept {
			KENGINE_PROFILING_SCOPE;

			if constexpr (matcher::has_member_c_str<Member>()) {
				return strstr(member.c_str(), str);
			}
			// else if constexpr (std::is_same_v<Member, const char *>::value)
			// 	ImGui::LabelText(name, member);

			else if constexpr (putils::is_iterable<Member>()
#ifdef KENGINE_LUA
							   && !std::is_same<sol::object, Member>()
							   && !std::is_base_of<sol::object, Member>()
#endif
				) {
				for (const auto & val: member)
					if (matchAttribute(val, str))
						return true;
				return false;
			}

			else if constexpr (putils::reflection::has_attributes<Member>() || putils::reflection::has_parents<Member>()) {
				bool matches = false;
				putils::reflection::for_each_attribute(member, [&](const auto & attr) noexcept {
					matches |= matchAttribute(attr.member, str);
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

	template<typename ... Comps>
	void registerMatchString(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		registerMetaComponentImplementation<meta::MatchString, Comps...>(
			r, [](const auto t, entt::const_handle e, const char * str) noexcept {
				KENGINE_PROFILING_SCOPE;

				using Comp = putils_wrapped_type(t);

				if constexpr (std::is_empty<Comp>())
					return false;
				else {
					const auto comp = e.try_get<Comp>();
					if (!comp)
						return false;

					if (strstr(putils::reflection::get_class_name<Comp>(), str))
						return true;

					bool matches = false;
					putils::reflection::for_each_attribute(*comp, [&](const auto & attr) noexcept {
						if (impl::matchAttribute(attr.member, str))
							matches = true;
					});
					return matches;
				}
			}
		);
	}
}

