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

#ifndef KENGINE_MAX_COMPONENT_FUNCTIONS
# define KENGINE_MAX_COMPONENT_FUNCTIONS 8
#endif

namespace kengine {
	namespace detail {
		using Mutex = std::shared_mutex;
		using ReadLock = std::shared_lock<Mutex>;
		using WriteLock = std::lock_guard<Mutex>;
	}

	struct FunctionMap {
		const char * name = nullptr;

		// In the following functions, `Func` must inherit from kengine::functions::BaseFunction, i.e. have:
		//		a `Signature` type alias for a function pointer type
		//		a `Signature funcPtr;` attribute
		template<typename Func>
		typename Func::Signature getFunction() const {
			detail::ReadLock l(_mutex);
			for (const auto & f : _funcs)
				if (f.id == putils::meta::type<Func>::index)
					return (typename Func::Signature)f.funcPtr;
			return nullptr;
		}

		template<typename Func>
		void registerFunction(Func func) {
			{
				detail::ReadLock l(_mutex);
				for (const auto & f : _funcs)
					if (f.id == putils::meta::type<Func>::index)
						return;
			}
			detail::WriteLock l(_mutex);
			_funcs.push_back(Function{ putils::meta::type<Func>::index, func.funcPtr });
		}

	private:
		struct Function {
			putils::meta::type_index id = -1;
			void * funcPtr = nullptr;
		};
		putils::vector<Function, KENGINE_MAX_COMPONENT_FUNCTIONS> _funcs;
		mutable detail::Mutex _mutex;
	};

	namespace detail {
		static constexpr size_t INVALID = (size_t)-1;

		struct MetadataBase {
			size_t id = detail::INVALID;
			virtual ~MetadataBase() = default;
			FunctionMap funcs;
		};

		struct GlobalCompMap {
			std::unordered_map<putils::meta::type_index, std::unique_ptr<MetadataBase>> map;
			detail::Mutex mutex;
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
		};

	public:
		static Comp & get(size_t id) { static auto & meta = metadata();
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

		static size_t id() {
			static const size_t ret = metadata().id;
			return ret;
		}

		template<typename Func>
		static auto getFunction() { static auto & meta = metadata();
			return meta.funcs.getFunction<Func>(name);
		}

		template<typename Func>
		static void registerFunction(Func func) { static auto & meta = metadata();
			meta.funcs.registerFunction(func);
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

				ptr->funcs.name = getName();
#ifndef KENGINE_NDEBUG
				std::cout << putils::termcolor::green << ptr->id << ' ' << putils::termcolor::cyan << ptr->funcs.name << '\n' << putils::termcolor::reset;
#endif
				return ptr;
			}();

			return *ret;
		}

		static const char * getName() {
			if constexpr (putils::reflection::has_class_name<Comp>::value)
				return putils::reflection::get_class_name<Comp>();
			else
				return typeid(Comp).name();
		}
	};
}
