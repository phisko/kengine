#include "register_everything.hpp"

// reflection
#include "putils/reflection.hpp"

// kengine data
#include "kengine/data/name.hpp"

// kengine meta
#include "kengine/meta/size.hpp"

#include "register_meta_components.hpp"
#include "register_with_script_languages.hpp"

namespace kengine {
	template<typename... Comps>
	void register_everything(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		register_with_script_languages<IsComponent, Comps...>(r);
		if constexpr (IsComponent) {
			register_meta_components<Comps...>(r);

			putils::for_each_type<Comps...>([&](auto && t) noexcept {
				using type = putils_wrapped_type(t);

				const auto e = type_helper::get_type_entity<type>(r);
				r.emplace<data::name>(e, putils::reflection::get_class_name<type>());
				r.emplace<meta::size>(e, sizeof(type));
			});
		}
	}
}