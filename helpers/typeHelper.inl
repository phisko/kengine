#include "typeHelper.hpp"

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

            const auto newTypeEntity = entities.create([](Entity & e) {
                e += impl::TypeEntityTag{ putils::meta::type<T>::index };
			});
            return newTypeEntity.id;
        }();

        return entities[ret];
    }
}
