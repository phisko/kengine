#pragma once

namespace kengine {
	struct HighlightComponent {
		putils::Vector3f colorNormalized;
		float intensity = .5f;

		pmeta_get_class_name(HighlightComponent);
	};
}