#include "typeHelper.hpp"
#include "logHelper.hpp"
#include "reflection.hpp"

namespace kengine::typeHelper {
    namespace impl {
        struct TypeEntityTag {
            putils::meta::type_index type;
        };
    }

    template <typename T>
    Entity getTypeEntity() noexcept {
        static EntityID ret = []() noexcept {
            for (const auto [e, comp] : entities.with<impl::TypeEntityTag>())
                if (comp.type == putils::meta::type<T>::index)
					return e.id;

            if (putils::reflection::has_class_name<T>())
                kengine_log(Log, "Init/TypeEntity", putils::reflection::get_class_name<T>());
            const auto newTypeEntity = entities.create([](Entity & e) {
                e += impl::TypeEntityTag{ putils::meta::type<T>::index };
			});
            return newTypeEntity.id;
        }();

        return entities[ret];
    }
}
