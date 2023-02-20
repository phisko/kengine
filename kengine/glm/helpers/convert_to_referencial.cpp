#include "convert_to_referencial.hpp"

// kengine glm
#include "kengine/glm/helpers/to_vec.hpp"

namespace kengine::glm {
	putils::point3f convert_to_referencial(const putils::point3f & pos, const ::glm::mat4 & conversion_matrix) noexcept {
		const auto tmp = conversion_matrix * ::glm::vec4(to_vec(pos), 1.f);
		return { tmp.x, tmp.y, tmp.z };
	}
}