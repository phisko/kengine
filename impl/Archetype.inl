#include "meta/for_each.hpp"
#include "no.hpp"

namespace kengine::impl {
	template<typename ... Comps>
	bool Archetype::matches() noexcept {
		{
			ReadLock l(mutex);
			if (entities.empty())
				return false;
		}

		bool good = true;
		putils::for_each_type<Comps...>([&](auto && type) noexcept {
			using T = putils_wrapped_type(type);

			if constexpr (is_not<T>()) {
				using CompType = typename T::CompType;
				const bool hasComp = mask.test(Component<CompType>::id());
				good &= !hasComp;
			}
			else {
				const bool hasComp = mask.test(Component<T>::id());
				good &= hasComp;
			}
			});

		if (good && !sorted) {
			WriteLock l(mutex);
			sort();
		}

		return good;
	}
}