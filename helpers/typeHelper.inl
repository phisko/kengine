#include "typeHelper.hpp"

namespace kengine::typeHelper {
    namespace impl {
        template<typename T>
        struct TypeEntityTag {};
    }

    template <typename T>
    Entity getTypeEntity() noexcept {
        static EntityID ret = []() noexcept {
            for (const auto [e, comp] : entities.with<impl::TypeEntityTag<T>>())
                return e.id;
            return entities.create([](Entity &) {}).id;
        }();
        return entities.get(ret);
    }
}
