#include "register_meta_components.hpp"

// stl
#include <algorithm>
#include <execution>

// putils
#include "putils/range.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

#include "impl/register_attributes.hpp"
#include "impl/register_count.hpp"
#include "impl/register_display_imgui.hpp"
#include "impl/register_edit_imgui.hpp"
#include "impl/register_emplace_or_replace.hpp"
#include "impl/register_for_each_entity.hpp"
#include "impl/register_get.hpp"
#include "impl/register_has.hpp"
#include "impl/register_load_from_json.hpp"
#include "impl/register_match_string.hpp"
#include "impl/register_remove.hpp"
#include "impl/register_save_to_json.hpp"

namespace kengine {
	template<typename... Comps>
	void register_meta_components(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		using component_registrator = void(entt::registry &);
		static constexpr component_registrator * component_registrators[] = {
			register_attributes<Comps...>,
			register_count<Comps...>,
			register_display_imgui<Comps...>,
			register_edit_imgui<Comps...>,
			register_emplace_or_replace<Comps...>,
			register_for_each_entity<Comps...>,
			register_get<Comps...>,
			register_has<Comps...>,
			register_load_from_json<Comps...>,
			register_match_string<Comps...>,
			register_remove<Comps...>,
			register_save_to_json<Comps...>,
		};

		// Pre-instatiate storages
		r.storage<meta::attributes>();
		r.storage<meta::count>();
		r.storage<meta::display_imgui>();
		r.storage<meta::edit_imgui>();
		r.storage<meta::emplace_or_replace>();
		r.storage<meta::emplace_or_replace_move>();
		r.storage<meta::for_each_entity>();
		r.storage<meta::for_each_entity_without>();
		r.storage<meta::get>();
		r.storage<meta::get_const>();
		r.storage<meta::has>();
		r.storage<meta::load_from_json>();
		r.storage<meta::match_string>();
		r.storage<meta::remove>();
		r.storage<meta::save_to_json>();

		std::for_each(std::execution::par_unseq, putils_range(component_registrators), [&](auto registrator) {
			registrator(r);
		});
	}
}
