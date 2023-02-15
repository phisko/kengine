#include "register_meta_components.hpp"

// stl
#include <algorithm>
#include <execution>
#include <vector>

// putils
#include "putils/range.hpp"
#include "putils/thread_name.hpp"

// kengine core
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine meta
#include "kengine/meta/helpers/impl/count.hpp"
#include "kengine/meta/helpers/impl/emplace_or_replace.hpp"
#include "kengine/meta/helpers/impl/for_each_entity.hpp"
#include "kengine/meta/helpers/impl/get.hpp"
#include "kengine/meta/helpers/impl/has.hpp"
#include "kengine/meta/helpers/impl/match_string.hpp"
#include "kengine/meta/helpers/impl/remove.hpp"

// kengine meta/imgui
#include "kengine/meta/imgui/helpers/impl/display_imgui.hpp"
#include "kengine/meta/imgui/helpers/impl/edit_imgui.hpp"

// kengine meta/json
#include "kengine/meta/json/helpers/impl/load_from_json.hpp"
#include "kengine/meta/json/helpers/impl/save_to_json.hpp"

namespace kengine {
	template<typename... Comps>
	void register_meta_components(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "register_meta_components", "Registering meta components");

		using component_registrator = void(entt::registry &);
		std::vector<component_registrator *> registrators;

		putils::for_each_type<
			meta::count,
			meta::display_imgui,
			meta::edit_imgui,
			meta::emplace_or_replace, meta::emplace_or_replace_move,
			meta::for_each_entity, meta::for_each_entity_without,
			meta::get, meta::get_const,
			meta::has,
			meta::load_from_json,
			meta::match_string,
			meta::remove,
			meta::save_to_json>([&](auto t) {
			using type = putils_wrapped_type(t);

			kengine_logf(r, verbose, "register_meta_components", "Pre-instantiating storage for %s", putils::reflection::get_class_name<type>());
			r.storage<type>();
			registrators.emplace_back(register_meta_component_implementation<type, Comps...>);
		});

		std::for_each(std::execution::par_unseq, putils_range(registrators), [&](auto registrator) {
			const putils::scoped_thread_name thread_name("Type registrator");
			registrator(r);
		});
	}
}