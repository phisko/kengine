#pragma once

#include "EntityManager.hpp"
#include "json.hpp"
#include "magic_enum.hpp"

#include "functions/LoadFromJSON.hpp"

namespace kengine {
	template<typename Comp>
	void registerComponentJSONLoader(kengine::EntityManager & em);

	template<typename ... Comps>
	void registerComponentJSONLoaders(kengine::EntityManager & em);
}

namespace kengine {
	namespace detail {
		namespace componentJSONLoader {
			putils_member_detector(c_str);
		}
		template<typename MemberRef>
		static void loadJSONObject(const putils::json & object, MemberRef && member) {
			using Member = std::remove_reference_t<MemberRef>;

			if constexpr (componentJSONLoader::has_member_c_str<Member>::value)
				member = object.get<std::string>().c_str();
			else if constexpr (putils::is_std_vector<Member>::value) {
				for (const auto & it : object) {
					member.emplace_back();
					auto & element = member.back();
					loadJSONObject(it, element);
				}
			}
			else if constexpr (putils::is_vector<Member>::value) {
				for (const auto & it : object) {
					auto & element = member.emplace_back();
					loadJSONObject(it, element);
				}
			}
			else if constexpr (std::is_array<Member>::value) {
				size_t i = 0;
				assert(object.array().size() <= lengthof(member));
				for (const auto & it : object) {
					auto & element = member[i];
					loadJSONObject(it, element);
					++i;
				}
			}
			else if constexpr (putils::reflection::has_attributes<Member>::value) {
				putils::reflection::for_each_attribute<Member>([&member, &object](const char * name, const auto attr) {
					const auto attrJSON = object.find(name);
					if (attrJSON != object.end())
						loadJSONObject(*attrJSON, member.*attr);
				});
			}
			else if constexpr (std::is_enum<Member>::value) {
				for (const auto & p : putils::magic_enum::enum_entries<Member>())
					if (object == putils::string<64>(p.second).c_str())
						member = p.first;
			}
			else if constexpr (std::is_scalar<Member>::value)
				member = object;
		}

		template<typename Component>
		static void loadJSONComponent(const putils::json & jsonEntity, kengine::Entity & e) {
			const auto it = jsonEntity.find(putils::reflection::get_class_name<Component>());
			if (it == jsonEntity.end())
				return;
			auto & comp = e.attach<Component>();
			loadJSONObject(*it, comp);
		}
	}

	template<typename Comp>
	void registerComponentJSONLoader(kengine::EntityManager & em) {
		em.registerComponentFunction<Comp>(kengine::functions::LoadFromJSON{ detail::loadJSONComponent<Comp> });
	}

	template<typename ... Comps>
	void registerComponentJSONLoaders(kengine::EntityManager & em) {
		putils_for_each_type(Comps, [&](auto type) {
			using Type = putils_wrapped_type(type);
			registerComponentJSONLoader<Type>(em);
		});
	}
}