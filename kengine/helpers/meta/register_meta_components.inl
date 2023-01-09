#include "register_meta_components.hpp"

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

		register_attributes<Comps...>(r);
		register_count<Comps...>(r);
		register_display_imgui<Comps...>(r);
		register_edit_imgui<Comps...>(r);
		register_emplace_or_replace<Comps...>(r);
		register_for_each_entity<Comps...>(r);
		register_get<Comps...>(r);
		register_has<Comps...>(r);
		register_load_from_json<Comps...>(r);
		register_match_string<Comps...>(r);
		register_remove<Comps...>(r);
		register_save_to_json<Comps...>(r);
	}
}
