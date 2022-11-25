#include "type_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/on_scope_exit.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::type_helper {
    namespace impl {
        struct type_entity_tag {
            putils::meta::type_index type;
        };
    }

    template <typename T>
    entt::entity get_type_entity(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

        static const auto init = [&](entt::entity & to_reset) noexcept {
            for (const auto [e, comp] : r.view<impl::type_entity_tag>().each())
                if (comp.type == putils::meta::type<T>::index)
                    return e;

            if (putils::reflection::has_class_name<T>())
                kengine_log(r, log, "Init/TypeEntity", putils::reflection::get_class_name<T>());

			const auto entity = r.create();
			r.emplace<impl::type_entity_tag>(entity, putils::meta::type<T>::index);

			// Reset the static variable to entt::null when the type entity is destroyed
			const auto reset_type_entity = [&] { to_reset = entt::null; };
			r.emplace<putils::on_scope_exit<decltype(reset_type_entity)>>(entity, reset_type_entity);
            return entity;
        };

        static entt::entity ret = entt::null;
		if (ret == entt::null)
			ret = init(ret);
		return ret;
    }
}
