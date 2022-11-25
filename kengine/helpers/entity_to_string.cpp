#include "entity_to_string.hpp"

namespace putils {
	std::string to_string(const entt::entity & e) noexcept {
		const auto non_enum_value = entt::id_type(e);
		return to_string(non_enum_value);
	}

	template<>
	void parse(entt::entity & e, std::string_view str) noexcept {
		auto non_enum_value = entt::id_type(e);
		parse(non_enum_value, str);
		e = entt::entity(non_enum_value);
	}
}