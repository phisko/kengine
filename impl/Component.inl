#include "kengine.hpp"
#include "Component.hpp"
#include "ComponentMask.hpp"

#ifndef KENGINE_NDEBUG
# include <iostream>
# include "termcolor.hpp"
# include "reflection.hpp"
#endif

namespace kengine::impl {
	template<typename Comp>
	void Metadata<Comp>::reset(ID entity) noexcept {
		auto & val = Component<Comp>::get(entity);
		val.~Comp();
		new (&val) Comp;
	}

	template<typename Comp>
	Comp & Component<Comp>::get(ID entity) noexcept {
		if constexpr (std::is_empty<Comp>()) {
			static Comp ret;
			return ret;
		}
		else {
			static auto & meta = metadata();
			ReadLock r(meta._mutex);
			const auto it = meta._map.find(entity);
			if (it != meta._map.end())
				return it->second;
			r.unlock();

			WriteLock l(meta._mutex);
			return meta._map[entity];
		}
	}

	template<typename Comp>
	ID Component<Comp>::id() noexcept {
		static const size_t ret = metadata().id;
		return ret;
	}

	template<typename Comp>
	Metadata<Comp> & Component<Comp>::metadata() noexcept {
		static Metadata<Comp> * ret = []() noexcept {
			const auto typeIndex = putils::meta::type<Comp>::index;

			{
				ReadLock l(state->_componentsMutex);
				const auto it = std::find_if(state->_components.begin(), state->_components.end(),
					[typeIndex](const std::unique_ptr<ComponentMetadata> & meta) noexcept { return meta->type == typeIndex; }
				);
				if (it != state->_components.end())
					return static_cast<Metadata<Comp> *>(it->get());
			}

			auto tmp = std::make_unique<Metadata<Comp>>();
			tmp->type = typeIndex;
			auto ptr = static_cast<Metadata<Comp> *>(tmp.get());
			{
				WriteLock l(state->_componentsMutex);
				state->_components.push_back(std::move(tmp));
				ptr->id = state->_components.size() - 1;
				assert(ptr->id < KENGINE_COMPONENT_COUNT);
			}

#ifndef KENGINE_NDEBUG
			std::cout << putils::termcolor::cyan;
			std::cout << putils::reflection::get_class_name<Comp>() << '\t' << ptr->id << '\n';
			std::cout << putils::termcolor::reset;
#endif
			return ptr;
		}();

		return *ret;
	}
}
