#include "registerComponentMatcher.hpp"

#include "kengine.hpp"
#include "meta/MatchString.hpp"
#include "helpers/typeHelper.hpp"
#include "sol.hpp"

namespace kengine {
	namespace impl {
		namespace matcher {
			putils_member_detector(c_str);
		}

		template<typename Member>
		static bool matchAttribute(const Member & member, const char * str) noexcept {
			if constexpr (matcher::has_member_c_str<Member>()) {
				return strstr(member.c_str(), str);
			}
			// else if constexpr (std::is_same_v<Member, const char *>::value)
			// 	ImGui::LabelText(name, member);

			else if constexpr (putils::is_iterable<Member>() &&
				!std::is_same<sol::object, Member>() &&
				!std::is_base_of<sol::object, Member>()) {
				for (const auto & val : member)
					if (matchAttribute(val, str))
						return true;
				return false;
			}

			else if constexpr (putils::reflection::has_attributes<Member>() || putils::reflection::has_parents<Member>()) {
				bool matches = false;
				putils::reflection::for_each_attribute(member, [&](const char * name, auto && attr) noexcept {
					matches |= matchAttribute(attr, str);
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
		static bool componentMatches(const Entity & e, const char * str) noexcept {
			if (strstr(putils::reflection::get_class_name<Comp>(), str))
				return true;

			bool matches = false;
			auto & comp = e.get<Comp>();
			putils::reflection::for_each_attribute(comp, [&](const char *name, auto &&member) noexcept {
				if (matchAttribute(member, str))
					matches = true;
			});
			return matches;
		}
	}

	template<typename ... Comps>
	void registerComponentMatcher() noexcept {
		putils::for_each_type<Comps...>([](auto t) noexcept {
			using Type = putils_wrapped_type(t);
			auto type = typeHelper::getTypeEntity<Type>();
			type += meta::MatchString{ impl::componentMatches<Type> };
		});
	}
}
