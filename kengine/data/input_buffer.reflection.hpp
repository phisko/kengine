#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::input_buffer
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(keys),
		putils_reflection_attribute(clicks),
		putils_reflection_attribute(moves),
		putils_reflection_attribute(scrolls)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::key_event),
		putils_reflection_type(refltype::click_event),
		putils_reflection_type(refltype::mouse_move_event),
		putils_reflection_type(refltype::mouse_scroll_event)
	);
};
#undef refltype

#define refltype kengine::data::input_buffer::key_event
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(window),
		putils_reflection_attribute(key),
		putils_reflection_attribute(pressed)
	);
};
#undef refltype

#define refltype kengine::data::input_buffer::click_event
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(window),
		putils_reflection_attribute(pos),
		putils_reflection_attribute(button),
		putils_reflection_attribute(pressed)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::point2f)
	);
};
#undef refltype

#define refltype kengine::data::input_buffer::mouse_move_event
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(window),
		putils_reflection_attribute(pos),
		putils_reflection_attribute(rel)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::point2f)
	);
};
#undef refltype

#define refltype kengine::data::input_buffer::mouse_scroll_event
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(window),
		putils_reflection_attribute(xoffset),
		putils_reflection_attribute(yoffset),
		putils_reflection_attribute(pos)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::point2f)
	);
};
#undef refltype