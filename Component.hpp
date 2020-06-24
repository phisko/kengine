#pragma once

#ifndef KENGINE_COMPONENT_CHUNK_SIZE
# define KENGINE_COMPONENT_CHUNK_SIZE 64
#endif

#ifndef KENGINE_NDEBUG
#include <iostream>
#endif

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <fstream>
#include <cstddef>
#include <unordered_map>
#include <memory>
#include <vector>
#include "meta/type.hpp"
#include "reflection.hpp"
#include "string.hpp"
#include "vector.hpp"
#include "termcolor.hpp"

namespace kengine {
	namespace detail {
		using Mutex = std::shared_mutex;
		using ReadLock = std::shared_lock<Mutex>;
		using WriteLock = std::lock_guard<Mutex>;
	}

	namespace detail {
		static constexpr size_t INVALID = (size_t)-1;

		struct MetadataBase {
			size_t id = detail::INVALID;
			size_t typeEntityID = detail::INVALID;
			virtual void reset(size_t id) = 0;
			virtual ~MetadataBase() = default;
		};

		struct GlobalCompMap {
			// Members use this _horrible_ "heap reference" pattern because components in the 
			// global pools may need to access these during program shutdown, after the
			// EntityManager (which contains this) has been destroyed, so this pattern lets them
			// stay alive. Sorry! Feel free to submit a pull request if you have a better solution
			std::unordered_map<putils::meta::type_index, std::unique_ptr<MetadataBase>> & map = *(new std::remove_reference_t<decltype(map)>);
			detail::Mutex & mutex = *(new std::remove_reference_t<decltype(mutex)>);
		};
		extern GlobalCompMap * components;
	}

	template<typename Comp>
	class Component {
	private:
		using Chunk = std::vector<Comp>;

		struct Metadata : detail::MetadataBase {
			std::vector<Chunk> chunks;
			mutable detail::Mutex _mutex;

			virtual void reset(size_t id) {
				auto & val = get(id);
				val = Comp{};
			}
		};

	public:
		static Comp & get(size_t id) {
			if constexpr (std::is_empty<Comp>()) {
				static Comp ret;
				return ret;
			}
			else {
				static auto & meta = metadata();
				detail::ReadLock r(meta._mutex);

				if (id >= meta.chunks.size() * KENGINE_COMPONENT_CHUNK_SIZE) {
					r.unlock(); { // Unlock read so we can get write
						detail::WriteLock l(meta._mutex);
						while (id >= meta.chunks.size() * KENGINE_COMPONENT_CHUNK_SIZE)
							meta.chunks.emplace_back();
						// Only populate the chunk we need
						meta.chunks.back().resize(KENGINE_COMPONENT_CHUNK_SIZE);
					} r.lock(); // Re-lock read
				}

				auto & currentChunk = meta.chunks[id / KENGINE_COMPONENT_CHUNK_SIZE];
				if (currentChunk.empty()) {
					r.unlock(); {
						detail::WriteLock l(meta._mutex);
						currentChunk.resize(KENGINE_COMPONENT_CHUNK_SIZE);
					} r.lock();
				}

				return currentChunk[id % KENGINE_COMPONENT_CHUNK_SIZE];
			}
		}

		static size_t id() {
			static const size_t ret = metadata().id;
			return ret;
		}

		template<typename Func>
		static size_t initTypeEntityID(Func && createEntity) {
			auto & meta = metadata();
			detail::ReadLock l(meta._mutex);
			if (meta.typeEntityID == detail::INVALID) {
				l.unlock();
				detail::WriteLock l2(meta._mutex);
				if (meta.typeEntityID == detail::INVALID) // Might have been set by another thread between unlock() and lock()
					meta.typeEntityID = createEntity();
			}
			return meta.typeEntityID;
		}

		template<typename Func>
		static size_t typeEntityID(Func && createEntity) {
			static size_t ret = initTypeEntityID(createEntity);
			return ret;
		}

		static void setTypeEntityID(size_t id) {
			metadata().typeEntityID = id;
		}

	private:
		static inline Metadata & metadata() {
			static Metadata * ret = [] {
				const auto typeIndex = putils::meta::type<Comp>::index;

				{
					detail::ReadLock l(detail::components->mutex);
					const auto it = detail::components->map.find(typeIndex);
					if (it != detail::components->map.end())
						return static_cast<Metadata *>(it->second.get());
				}

				auto tmp = std::make_unique<Metadata>();
				auto ptr = static_cast<Metadata *>(tmp.get());
				{
					detail::WriteLock l(detail::components->mutex);
					detail::components->map[typeIndex] = std::move(tmp);
					ptr->id = detail::components->map.size() - 1;
				}

#ifndef KENGINE_NDEBUG
				std::cout << putils::termcolor::green << ptr->id << ' ' << putils::termcolor::cyan << putils::reflection::get_class_name<Comp>() << '\n' << putils::termcolor::reset;
#endif
				return ptr;
			}();

			return *ret;
		}
	};
}
