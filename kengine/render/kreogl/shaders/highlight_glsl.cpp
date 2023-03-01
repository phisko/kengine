#include "highlight_glsl.hpp"

namespace kengine::render::kreogl {
	const char * highlight_glsl::frag = R"(
#version 330

uniform vec3 view_pos;
uniform vec2 screen_size;

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D guser_data;

uniform float entity_id;
uniform vec4 highlight_color;
uniform float intensity;

out vec4 out_color;

void main() {
	out_color = vec4(0.0);

   	vec2 tex_coord = gl_FragCoord.xy / screen_size;

    vec4 world_pos = texture(gposition, tex_coord);
	if (world_pos.w == 0) // Empty pixel
		return;

	vec3 normal = -texture(gnormal, tex_coord).xyz;
	if (texture(guser_data, tex_coord).x != entity_id)
		return;

	float highlight = 0.0;
	float distance_to_entity = length(world_pos.xyz - view_pos);

	for (float x = -intensity; x <= intensity; x += 1.0)
		for (float y = -intensity; y <= intensity; y += 1.0) {
			vec2 neighbor_coord = vec2(gl_FragCoord.x + x, gl_FragCoord.y + y) / screen_size;

			vec4 neighbor_pos = texture(gposition, neighbor_coord);
			bool empty_neighbor = neighbor_pos.w == 0;
			bool neighbor_closer = length(neighbor_pos.xyz - view_pos) < distance_to_entity;
			if (!empty_neighbor && neighbor_closer) // This pixel is an object closer than entity_id
				continue;

			if (texture(guser_data, neighbor_coord).x != entity_id)
				highlight = 1.0;
		}

	out_color = highlight * highlight_color;
})";
}