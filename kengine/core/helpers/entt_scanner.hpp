#pragma once

// scn
#include <scn/scn.h>

// entt
#include <entt/entity/entity.hpp>
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

namespace scn {
	template<>
	struct scanner<entt::entity> : scn::empty_parser {
		template <typename Context>
		error scan(entt::entity & val, Context & ctx) {
			std::underlying_type_t<entt::entity> non_enum_value;
			const auto result = scn::scan_usertype(ctx, "[{}]", non_enum_value);
			if (result)
				val = entt::entity(non_enum_value);
			return result;
		}
	};
}
