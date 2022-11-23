#include "typeHelper.hpp"

// entt
#include <entt/entity/registry.hpp>

// reflection
#include "reflection.hpp"

// kengine functions
#include "functions/OnTerminate.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::typeHelper {
    namespace impl {
        struct TypeEntityTag {
            putils::meta::type_index type;
        };
    }

    template <typename T>
    entt::entity getTypeEntity(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

        static const auto init = [&](entt::entity & toReset) noexcept {
            for (const auto [e, comp] : r.view<impl::TypeEntityTag>().each())
                if (comp.type == putils::meta::type<T>::index)
                    return e;

            if (putils::reflection::has_class_name<T>())
                kengine_log(r, Log, "Init/TypeEntity", putils::reflection::get_class_name<T>());

			const auto entity = r.create();
			r.emplace<impl::TypeEntityTag>(entity, putils::meta::type<T>::index);
			r.emplace<functions::OnTerminate>(entity, [&] { toReset = entt::null; });
            return entity;
        };

        static entt::entity ret = entt::null;
		if (ret == entt::null)
			ret = init(ret);
		return ret;
    }
}
