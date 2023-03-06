#include "register_meta_components.hpp"

// stl
#include <algorithm>
#include <execution>
#include <vector>

// putils
#include "putils/range.hpp"
#include "putils/thread_name.hpp"

// kengine
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/meta/helpers/register_meta_component_implementation.hpp"
#include "kengine/meta/helpers/impl/count.hpp"
#include "kengine/meta/helpers/impl/emplace_or_replace.hpp"
#include "kengine/meta/helpers/impl/for_each_entity.hpp"
#include "kengine/meta/helpers/impl/get.hpp"
#include "kengine/meta/helpers/impl/has.hpp"
#include "kengine/meta/helpers/impl/match_string.hpp"
#include "kengine/meta/helpers/impl/remove.hpp"

#ifdef KENGINE_META_IMGUI
#include "kengine/meta/imgui/helpers/impl/display.hpp"
#include "kengine/meta/imgui/helpers/impl/edit.hpp"
#endif

#ifdef KENGINE_META_JSON
#include "kengine/meta/json/helpers/impl/load.hpp"
#include "kengine/meta/json/helpers/impl/save.hpp"
#endif

namespace kengine::meta {
	template<typename... Comps>
	void register_meta_components(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "meta", "Registering meta components");

		using component_registrator = void(entt::registry &);
		std::vector<component_registrator *> registrators;

		putils::for_each_type<
#ifdef KENGINE_META_IMGUI
			meta::imgui::display,
			meta::imgui::edit,
#endif
#ifdef KENGINE_META_JSON
			meta::json::load,
			meta::json::save,
#endif
			meta::count,
			meta::emplace_or_replace, meta::emplace_or_replace_move,
			meta::for_each_entity, meta::for_each_entity_without,
			meta::get, meta::get_const,
			meta::has,
			meta::match_string,
			meta::remove>([&](auto t) {
			using type = putils_wrapped_type(t);

			kengine_logf(r, verbose, "meta", "Pre-instantiating storage for {}", putils::reflection::get_class_name<type>());
			r.storage<type>();
			registrators.emplace_back(register_meta_component_implementation<type, Comps...>);
		});

		std::for_each(std::execution::par_unseq, putils_range(registrators), [&](auto registrator) {
			const putils::scoped_thread_name thread_name("Type registrator");
			registrator(r);
		});
	}
}