#pragma once

namespace kengine {
	struct MeshComponent {
		// Entity which had a MeshLoaderComponent
		kengine::Entity::ID meshInfo = kengine::Entity::INVALID_ID;
	};
}