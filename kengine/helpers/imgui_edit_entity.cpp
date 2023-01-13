#include "imgui_edit_entity.hpp"

namespace putils::reflection {
	template<>
	void imgui_edit(const char * name, entt::entity & obj) noexcept {
		auto non_enum_value = entt::id_type(obj);
		imgui_edit(name, non_enum_value);
		obj = entt::entity(non_enum_value);
	}

	template<>
	void imgui_edit(const char * name, const entt::entity & obj) noexcept {
		const auto non_enum_value = static_cast<std::underlying_type_t<entt::entity>>(obj);
		imgui_edit(name, non_enum_value);
	}
}