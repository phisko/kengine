#pragma once

#include "EntityManager.hpp"
#include "components/TypeComponent.hpp"
#include "vector.hpp"

namespace kengine::TypeHelper {
	template <typename T>
	Entity getTypeEntity(EntityManager & em);

	// Returns a container of tuple<Entity, TypeComponent *, Comps *>
	template<typename ... Comps>
	auto getSortedTypeEntities(EntityManager & em); 
}

// Impl
namespace kengine::TypeHelper {
    template <typename T>
    Entity getTypeEntity(EntityManager & em) {
        static Entity::ID ret = Component<T>::typeEntityID([&] {
            return em.createEntity([](Entity &e) {
                    e += TypeComponent{putils::reflection::get_class_name<T>()};
                }).id;
        });
        return em.getEntity(ret);
    }

	namespace detail {
		template<typename PointerTuple, typename RefTuple, size_t I, size_t ...Is>
		void setImpl(PointerTuple & p, const RefTuple & r, std::index_sequence<I, Is...>) {
			std::get<I + 1>(p) = &std::get<I + 1>(r);
			setImpl(p, r, std::index_sequence<Is...>());
		}

		template<typename PointerTuple, typename RefTuple>
		void setImpl(PointerTuple & p, const RefTuple & r, std::index_sequence<>) {}

		template<typename PointerTuple, typename RefTuple, size_t ...Is>
		void set(PointerTuple & p, const RefTuple & r, std::index_sequence<Is...> is) {
			std::get<0>(p) = std::get<0>(r); // 0 is Entity
			setImpl(p, r, is);
		}
	}

	template<typename ... Comps>
	auto getSortedTypeEntities(EntityManager & em) {
		using Type = std::tuple<Entity, const TypeComponent *, const Comps *...>;

		putils::vector<Type, KENGINE_COMPONENT_COUNT> ret;

		for (const auto & t : em.getEntities<TypeComponent, Comps...>()) {
			ret.emplace_back();
			detail::set(ret.back(), t, std::make_index_sequence<sizeof...(Comps) + 1>());
		}
		std::sort(ret.begin(), ret.end(), [&](const auto & lhs, const auto & rhs) {
			return strcmp(std::get<1>(lhs)->name, std::get<1>(rhs)->name) < 0;
		});

		return ret;
	}
}