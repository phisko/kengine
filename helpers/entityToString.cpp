#include "entityToString.hpp"

namespace putils {
	template<>
	std::string toString(const entt::entity & e) noexcept {
		const auto nonEnumValue = entt::id_type(e);
		return toString(nonEnumValue);
	}

	template<>
	void parse(entt::entity & e, std::string_view str) noexcept {
		auto nonEnumValue = entt::id_type(e);
		parse(nonEnumValue, str);
		e = entt::entity(nonEnumValue);
	}
}