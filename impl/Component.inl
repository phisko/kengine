#include "kengine.hpp"
#include "Component.hpp"

#ifndef KENGINE_NDEBUG
# include <iostream>
# include "termcolor.hpp"
# include "reflection.hpp"
#endif

namespace kengine::impl {
	template<typename Comp>
	void Metadata<Comp>::reset(ID entity) noexcept {
		auto & val = Component<Comp>::get(entity);
		val = Comp();
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

			if (entity >= meta.chunks.size() * KENGINE_COMPONENT_CHUNK_SIZE) {
				r.unlock(); { // Unlock read so we can get write
					WriteLock l(meta._mutex);
					while (entity >= meta.chunks.size() * KENGINE_COMPONENT_CHUNK_SIZE)
						meta.chunks.emplace_back();
					// Only populate the chunk we need
					meta.chunks.back().resize(KENGINE_COMPONENT_CHUNK_SIZE);
				} r.lock(); // Re-lock read
			}

			auto & currentChunk = meta.chunks[entity / KENGINE_COMPONENT_CHUNK_SIZE];
			if (currentChunk.empty()) {
				r.unlock(); {
					WriteLock l(meta._mutex);
					currentChunk.resize(KENGINE_COMPONENT_CHUNK_SIZE);
				} r.lock();
			}

			return currentChunk[entity % KENGINE_COMPONENT_CHUNK_SIZE];
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
				const auto it = state->_components.find(typeIndex);
				if (it != state->_components.end())
					return static_cast<Metadata<Comp> *>(it->second.get());
			}

			auto tmp = std::make_unique<Metadata<Comp>>();
			auto ptr = static_cast<Metadata<Comp> *>(tmp.get());
			{
				WriteLock l(state->_componentsMutex);
				state->_components[typeIndex] = std::move(tmp);
				ptr->id = state->_components.size() - 1;
			}

#ifndef KENGINE_NDEBUG
			std::cout << putils::termcolor::green << ptr->id << ' ' << putils::termcolor::cyan << putils::reflection::get_class_name<Comp>() << '\n' << putils::termcolor::reset;
#endif
			return ptr;
		}();

		return *ret;
	}
}
