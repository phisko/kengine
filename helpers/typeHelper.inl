#include "typeHelper.hpp"

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
    Entity getTypeEntity() noexcept {
		KENGINE_PROFILING_SCOPE;

        static const auto init = [](EntityID & toReset) noexcept {
            for (const auto [e, comp] : entities.with<impl::TypeEntityTag>())
                if (comp.type == putils::meta::type<T>::index)
                    return e.id;

            if (putils::reflection::has_class_name<T>())
                kengine_log(Log, "Init/TypeEntity", putils::reflection::get_class_name<T>());

            const auto newTypeEntity = entities.create([&](Entity & e) {
                e += impl::TypeEntityTag{ putils::meta::type<T>::index };
                e += functions::OnTerminate{ [&] { toReset = INVALID_ID; }};
            });

            return newTypeEntity.id;
        };

        static EntityID ret = init(ret);
        if (ret == INVALID_ID)
            ret = init(ret);

        return entities[ret];
    }
}
