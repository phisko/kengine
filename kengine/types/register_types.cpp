
#include "register_types.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_types(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "Init", "Registering types");

		extern void register_kengine_data_adjustable(entt::registry &) noexcept;
		register_kengine_data_adjustable(r);
		extern void register_kengine_data_animation(entt::registry &) noexcept;
		register_kengine_data_animation(r);
		extern void register_kengine_data_animation_files(entt::registry &) noexcept;
		register_kengine_data_animation_files(r);
		extern void register_kengine_data_model_animation(entt::registry &) noexcept;
		register_kengine_data_model_animation(r);
		extern void register_kengine_functions_appears_in_viewport(entt::registry &) noexcept;
		register_kengine_functions_appears_in_viewport(r);
		extern void register_kengine_data_camera(entt::registry &) noexcept;
		register_kengine_data_camera(r);
		extern void register_kengine_data_command_line(entt::registry &) noexcept;
		register_kengine_data_command_line(r);
		extern void register_kengine_data_viewport(entt::registry &) noexcept;
		register_kengine_data_viewport(r);
		extern void register_kengine_data_window(entt::registry &) noexcept;
		register_kengine_data_window(r);
		extern void register_kengine_data_debug_graphics(entt::registry &) noexcept;
		register_kengine_data_debug_graphics(r);
		extern void register_kengine_functions_execute(entt::registry &) noexcept;
		register_kengine_functions_execute(r);
		extern void register_kengine_data_god_rays(entt::registry &) noexcept;
		register_kengine_data_god_rays(r);
		extern void register_kengine_data_graphics(entt::registry &) noexcept;
		register_kengine_data_graphics(r);
		extern void register_kengine_data_highlight(entt::registry &) noexcept;
		register_kengine_data_highlight(r);
		extern void register_kengine_data_imgui_tool(entt::registry &) noexcept;
		register_kengine_data_imgui_tool(r);
		extern void register_kengine_data_input(entt::registry &) noexcept;
		register_kengine_data_input(r);
		extern void register_kengine_data_instance(entt::registry &) noexcept;
		register_kengine_data_instance(r);
		extern void register_kengine_data_dir_light(entt::registry &) noexcept;
		register_kengine_data_dir_light(r);
		extern void register_kengine_data_point_light(entt::registry &) noexcept;
		register_kengine_data_point_light(r);
		extern void register_kengine_data_spot_light(entt::registry &) noexcept;
		register_kengine_data_spot_light(r);
		extern void register_kengine_data_lua(entt::registry &) noexcept;
		register_kengine_data_lua(r);
		extern void register_kengine_data_lua_table(entt::registry &) noexcept;
		register_kengine_data_lua_table(r);
		extern void register_kengine_data_model_collider(entt::registry &) noexcept;
		register_kengine_data_model_collider(r);
		extern void register_kengine_data_model(entt::registry &) noexcept;
		register_kengine_data_model(r);
		extern void register_kengine_data_name(entt::registry &) noexcept;
		register_kengine_data_name(r);
		extern void register_kengine_data_nav_mesh(entt::registry &) noexcept;
		register_kengine_data_nav_mesh(r);
		extern void register_kengine_functions_on_click(entt::registry &) noexcept;
		register_kengine_functions_on_click(r);
		extern void register_kengine_data_pathfinding(entt::registry &) noexcept;
		register_kengine_data_pathfinding(r);
		extern void register_kengine_data_physics(entt::registry &) noexcept;
		register_kengine_data_physics(r);
		extern void register_kengine_data_python(entt::registry &) noexcept;
		register_kengine_data_python(r);
		extern void register_kengine_data_selected(entt::registry &) noexcept;
		register_kengine_data_selected(r);
		extern void register_kengine_data_model_skeleton(entt::registry &) noexcept;
		register_kengine_data_model_skeleton(r);
		extern void register_kengine_data_sky_box(entt::registry &) noexcept;
		register_kengine_data_sky_box(r);
		extern void register_kengine_data_sprite_2d(entt::registry &) noexcept;
		register_kengine_data_sprite_2d(r);
		extern void register_kengine_data_sprite_3d(entt::registry &) noexcept;
		register_kengine_data_sprite_3d(r);
		extern void register_kengine_data_text_2d(entt::registry &) noexcept;
		register_kengine_data_text_2d(r);
		extern void register_kengine_data_text_3d(entt::registry &) noexcept;
		register_kengine_data_text_3d(r);
		extern void register_kengine_data_time_modulator(entt::registry &) noexcept;
		register_kengine_data_time_modulator(r);
		extern void register_kengine_data_transform(entt::registry &) noexcept;
		register_kengine_data_transform(r);
		extern void register_kengine_meta_attach_to(entt::registry &) noexcept;
		register_kengine_meta_attach_to(r);
		extern void register_kengine_meta_attributes(entt::registry &) noexcept;
		register_kengine_meta_attributes(r);
		extern void register_kengine_meta_copy(entt::registry &) noexcept;
		register_kengine_meta_copy(r);
		extern void register_kengine_meta_count(entt::registry &) noexcept;
		register_kengine_meta_count(r);
		extern void register_kengine_meta_detach_from(entt::registry &) noexcept;
		register_kengine_meta_detach_from(r);
		extern void register_kengine_meta_display_imgui(entt::registry &) noexcept;
		register_kengine_meta_display_imgui(r);
		extern void register_kengine_meta_edit_imgui(entt::registry &) noexcept;
		register_kengine_meta_edit_imgui(r);
		extern void register_kengine_meta_for_each_entity(entt::registry &) noexcept;
		register_kengine_meta_for_each_entity(r);
		extern void register_kengine_meta_for_each_entity_without(entt::registry &) noexcept;
		register_kengine_meta_for_each_entity_without(r);
		extern void register_kengine_meta_get(entt::registry &) noexcept;
		register_kengine_meta_get(r);
		extern void register_kengine_meta_has(entt::registry &) noexcept;
		register_kengine_meta_has(r);
		extern void register_kengine_meta_load_from_json(entt::registry &) noexcept;
		register_kengine_meta_load_from_json(r);
		extern void register_kengine_meta_match_string(entt::registry &) noexcept;
		register_kengine_meta_match_string(r);
		extern void register_kengine_meta_save_to_json(entt::registry &) noexcept;
		register_kengine_meta_save_to_json(r);
		extern void register_kengine_meta_size(entt::registry &) noexcept;
		register_kengine_meta_size(r);
		extern void register_putils_point2f(entt::registry &) noexcept;
		register_putils_point2f(r);
	}
}
