#pragma once

namespace kengine {
	struct TexturedModelComponent : kengine::not_serializable {
		// Used to indicate that this entity's ModelComponent is tied to a textured model
		pmeta_get_class_name(TexturedModelComponent);
	};
}
