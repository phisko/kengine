
#include "add_type_registrator.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine functions
#include "kengine/functions/register_types.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	extern void register_kengine_data_adjustable(entt::registry &) noexcept;
	extern void register_kengine_data_animation(entt::registry &) noexcept;
	extern void register_kengine_data_animation_files(entt::registry &) noexcept;
	extern void register_kengine_data_camera(entt::registry &) noexcept;
	extern void register_kengine_data_collision(entt::registry &) noexcept;
	extern void register_kengine_data_command_line(entt::registry &) noexcept;
	extern void register_kengine_data_debug_graphics(entt::registry &) noexcept;
	extern void register_kengine_data_god_rays(entt::registry &) noexcept;
	extern void register_kengine_data_graphics(entt::registry &) noexcept;
	extern void register_kengine_data_highlight(entt::registry &) noexcept;
	extern void register_kengine_data_imgui_context(entt::registry &) noexcept;
	extern void register_kengine_data_imgui_scale(entt::registry &) noexcept;
	extern void register_kengine_data_imgui_tool(entt::registry &) noexcept;
	extern void register_kengine_data_input(entt::registry &) noexcept;
	extern void register_kengine_data_input_buffer(entt::registry &) noexcept;
	extern void register_kengine_data_instance(entt::registry &) noexcept;
	extern void register_kengine_data_keep_alive(entt::registry &) noexcept;
	extern void register_kengine_data_kinematic(entt::registry &) noexcept;
	extern void register_kengine_data_dir_light(entt::registry &) noexcept;
	extern void register_kengine_data_point_light(entt::registry &) noexcept;
	extern void register_kengine_data_spot_light(entt::registry &) noexcept;
	extern void register_kengine_data_lua(entt::registry &) noexcept;
	extern void register_kengine_data_lua_state(entt::registry &) noexcept;
	extern void register_kengine_data_lua_table(entt::registry &) noexcept;
	extern void register_kengine_data_model(entt::registry &) noexcept;
	extern void register_kengine_data_model_animation(entt::registry &) noexcept;
	extern void register_kengine_data_model_collider(entt::registry &) noexcept;
	extern void register_kengine_data_model_data(entt::registry &) noexcept;
	extern void register_kengine_data_model_skeleton(entt::registry &) noexcept;
	extern void register_kengine_data_name(entt::registry &) noexcept;
	extern void register_kengine_data_nav_mesh(entt::registry &) noexcept;
	extern void register_kengine_data_no_shadow(entt::registry &) noexcept;
	extern void register_kengine_data_on_screen(entt::registry &) noexcept;
	extern void register_kengine_data_pathfinding(entt::registry &) noexcept;
	extern void register_kengine_data_physics(entt::registry &) noexcept;
	extern void register_kengine_data_python(entt::registry &) noexcept;
	extern void register_kengine_data_python_state(entt::registry &) noexcept;
	extern void register_kengine_data_selected(entt::registry &) noexcept;
	extern void register_kengine_data_skeleton(entt::registry &) noexcept;
	extern void register_kengine_data_sky_box(entt::registry &) noexcept;
	extern void register_kengine_data_sprite_2d(entt::registry &) noexcept;
	extern void register_kengine_data_sprite_3d(entt::registry &) noexcept;
	extern void register_kengine_data_text_2d(entt::registry &) noexcept;
	extern void register_kengine_data_text_3d(entt::registry &) noexcept;
	extern void register_kengine_data_time_modulator(entt::registry &) noexcept;
	extern void register_kengine_data_transform(entt::registry &) noexcept;
	extern void register_kengine_data_viewport(entt::registry &) noexcept;
	extern void register_kengine_data_window(entt::registry &) noexcept;
	extern void register_kengine_functions_appears_in_viewport(entt::registry &) noexcept;
	extern void register_kengine_functions_execute(entt::registry &) noexcept;
	extern void register_kengine_functions_get_entity_in_pixel(entt::registry &) noexcept;
	extern void register_kengine_functions_get_position_in_pixel(entt::registry &) noexcept;
	extern void register_kengine_functions_log(entt::registry &) noexcept;
	extern void register_kengine_functions_on_click(entt::registry &) noexcept;
	extern void register_kengine_functions_on_collision(entt::registry &) noexcept;
	extern void register_kengine_functions_on_mouse_captured(entt::registry &) noexcept;
	extern void register_kengine_functions_query_position(entt::registry &) noexcept;
	extern void register_kengine_functions_register_types(entt::registry &) noexcept;
	extern void register_kengine_meta_attributes(entt::registry &) noexcept;
	extern void register_kengine_meta_count(entt::registry &) noexcept;
	extern void register_kengine_meta_display_imgui(entt::registry &) noexcept;
	extern void register_kengine_meta_edit_imgui(entt::registry &) noexcept;
	extern void register_kengine_meta_emplace_or_replace(entt::registry &) noexcept;
	extern void register_kengine_meta_emplace_or_replace_move(entt::registry &) noexcept;
	extern void register_kengine_meta_for_each_entity(entt::registry &) noexcept;
	extern void register_kengine_meta_for_each_entity_without(entt::registry &) noexcept;
	extern void register_kengine_meta_get(entt::registry &) noexcept;
	extern void register_kengine_meta_get_const(entt::registry &) noexcept;
	extern void register_kengine_meta_has(entt::registry &) noexcept;
	extern void register_kengine_meta_load_from_json(entt::registry &) noexcept;
	extern void register_kengine_meta_match_string(entt::registry &) noexcept;
	extern void register_kengine_meta_remove(entt::registry &) noexcept;
	extern void register_kengine_meta_save_to_json(entt::registry &) noexcept;
	extern void register_kengine_meta_size(entt::registry &) noexcept;

	void add_type_registrator(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto e = r.create();
		r.emplace<kengine::functions::register_types>(e, [](entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "Registering types");

			register_kengine_data_adjustable(r);
			register_kengine_data_animation(r);
			register_kengine_data_animation_files(r);
			register_kengine_data_camera(r);
			register_kengine_data_collision(r);
			register_kengine_data_command_line(r);
			register_kengine_data_debug_graphics(r);
			register_kengine_data_god_rays(r);
			register_kengine_data_graphics(r);
			register_kengine_data_highlight(r);
			register_kengine_data_imgui_context(r);
			register_kengine_data_imgui_scale(r);
			register_kengine_data_imgui_tool(r);
			register_kengine_data_input(r);
			register_kengine_data_input_buffer(r);
			register_kengine_data_instance(r);
			register_kengine_data_keep_alive(r);
			register_kengine_data_kinematic(r);
			register_kengine_data_dir_light(r);
			register_kengine_data_point_light(r);
			register_kengine_data_spot_light(r);
			register_kengine_data_lua(r);
			register_kengine_data_lua_state(r);
			register_kengine_data_lua_table(r);
			register_kengine_data_model(r);
			register_kengine_data_model_animation(r);
			register_kengine_data_model_collider(r);
			register_kengine_data_model_data(r);
			register_kengine_data_model_skeleton(r);
			register_kengine_data_name(r);
			register_kengine_data_nav_mesh(r);
			register_kengine_data_no_shadow(r);
			register_kengine_data_on_screen(r);
			register_kengine_data_pathfinding(r);
			register_kengine_data_physics(r);
			register_kengine_data_python(r);
			register_kengine_data_python_state(r);
			register_kengine_data_selected(r);
			register_kengine_data_skeleton(r);
			register_kengine_data_sky_box(r);
			register_kengine_data_sprite_2d(r);
			register_kengine_data_sprite_3d(r);
			register_kengine_data_text_2d(r);
			register_kengine_data_text_3d(r);
			register_kengine_data_time_modulator(r);
			register_kengine_data_transform(r);
			register_kengine_data_viewport(r);
			register_kengine_data_window(r);
			register_kengine_functions_appears_in_viewport(r);
			register_kengine_functions_execute(r);
			register_kengine_functions_get_entity_in_pixel(r);
			register_kengine_functions_get_position_in_pixel(r);
			register_kengine_functions_log(r);
			register_kengine_functions_on_click(r);
			register_kengine_functions_on_collision(r);
			register_kengine_functions_on_mouse_captured(r);
			register_kengine_functions_query_position(r);
			register_kengine_functions_register_types(r);
			register_kengine_meta_attributes(r);
			register_kengine_meta_count(r);
			register_kengine_meta_display_imgui(r);
			register_kengine_meta_edit_imgui(r);
			register_kengine_meta_emplace_or_replace(r);
			register_kengine_meta_emplace_or_replace_move(r);
			register_kengine_meta_for_each_entity(r);
			register_kengine_meta_for_each_entity_without(r);
			register_kengine_meta_get(r);
			register_kengine_meta_get_const(r);
			register_kengine_meta_has(r);
			register_kengine_meta_load_from_json(r);
			register_kengine_meta_match_string(r);
			register_kengine_meta_remove(r);
			register_kengine_meta_save_to_json(r);
			register_kengine_meta_size(r);
		});
	}
}