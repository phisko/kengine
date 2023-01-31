#include "register_meta_components.hpp"

// stl
#include <algorithm>
#include <execution>
#include <vector>

// putils
#include "putils/range.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"
#include "impl/count.hpp"
#include "impl/display_imgui.hpp"
#include "impl/edit_imgui.hpp"
#include "impl/emplace_or_replace.hpp"
#include "impl/for_each_entity.hpp"
#include "impl/get.hpp"
#include "impl/has.hpp"
#include "impl/load_from_json.hpp"
#include "impl/match_string.hpp"
#include "impl/remove.hpp"
#include "impl/save_to_json.hpp"

namespace kengine {
	template<typename... Comps>
	void register_meta_components(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

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

			// Pre-instantiate storage
			r.storage<type>();
			registrators.emplace_back(register_meta_component_implementation<type, Comps...>);
		});

		std::for_each(std::execution::par_unseq, putils_range(registrators), [&](auto registrator) {
			registrator(r);
		});
	}
}