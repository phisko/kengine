#include "kengine.hpp"
#include "Component.hpp"

#include <cassert>

#include "ComponentMask.hpp"
#include "impl/GlobalState.hpp"

#ifndef KENGINE_NDEBUG
# include <iostream>
# include <termcolor/termcolor.hpp>
# include "reflection.hpp"
#endif

namespace kengine::impl {
    template<typename Comp>
    Metadata<Comp>::~Metadata() noexcept {
        singleton = nullptr;
    }

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
			auto & meta = metadata();
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
		return metadata().id;
	}

	template<typename Comp>
	Metadata<Comp> & Component<Comp>::metadata() noexcept {
        static const auto init = [](Metadata<Comp> * & singleton) noexcept {
			const auto typeIndex = putils::meta::type<Comp>::index;

			{
				ReadLock l(kengine::impl::state->_componentsMutex);
				const auto it = std::find_if(kengine::impl::state->_components.begin(), kengine::impl::state->_components.end(),
					[typeIndex](const std::unique_ptr<ComponentMetadata> & meta) noexcept { return meta->type == typeIndex; }
				);
				if (it != kengine::impl::state->_components.end())
					return static_cast<Metadata<Comp> *>(it->get());
			}

			auto tmp = std::make_unique<Metadata<Comp>>(singleton);
			tmp->type = typeIndex;
			auto ptr = static_cast<Metadata<Comp> *>(tmp.get());
			{
				WriteLock l(kengine::impl::state->_componentsMutex);
				kengine::impl::state->_components.push_back(std::move(tmp));
				ptr->id = kengine::impl::state->_components.size() - 1;
				assert(ptr->id < KENGINE_COMPONENT_COUNT);
			}

#ifdef KENGINE_DEBUG_PRINT_COMPONENT_ID
			std::cout << termcolor::cyan;
			std::cout << putils::reflection::get_class_name<Comp>() << '\t' << ptr->id << std::endl;
			std::cout << termcolor::reset;
#endif

			return ptr;
		};

        static Metadata<Comp> * ret = nullptr;
        if (ret == nullptr) // handle re-init
            ret = init(ret);
		return *ret;
	}
}
