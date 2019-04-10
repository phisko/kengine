#pragma once

namespace kengine {
	struct ModelComponent {
		// Entity which had a ModelLoaderComponent
		kengine::Entity::ID modelInfo = kengine::Entity::INVALID_ID;
		pmeta_get_class_name(ModelComponent);
	};
}