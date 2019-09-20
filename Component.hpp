#pragma once

#ifndef KENGINE_COMPONENT_CHUNK_SIZE
# define KENGINE_COMPONENT_CHUNK_SIZE 64
#endif

#ifndef KENGINE_MAX_SAVE_PATH_LENGTH
# define KENGINE_MAX_SAVE_PATH_LENGTH 64
#endif

#ifndef NDEBUG
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
#include "reflection/Reflectible.hpp"
#include "not_serializable.hpp"
#include "string.hpp"
#include "vector.hpp"

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
				if (f.id == pmeta::type<Func>::index)
					return (typename Func::Signature)f.funcPtr;
			return nullptr;
		}

		template<typename Func>
		void registerFunction(Func func) {
			{
				detail::ReadLock l(_mutex);
				for (const auto & f : _funcs)
					if (f.id == pmeta::type<Func>::index)
						return;
			}
			detail::WriteLock l(_mutex);
			_funcs.push_back(Function{ pmeta::type<Func>::index, func.funcPtr });
		}

	private:
		struct Function {
			pmeta::type_index id = -1;
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
			virtual bool save(const char * directory) const = 0;
			virtual bool load(const char * directory) = 0;

			FunctionMap funcs;
		};

		struct GlobalCompMap {
			std::unordered_map<pmeta::type_index, std::unique_ptr<MetadataBase>> map;
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

			bool save(const char * directory) const final {
				if constexpr (putils::has_member_get_class_name<Comp>::value && !std::is_base_of<kengine::not_serializable, Comp>::value) {
					putils::string<KENGINE_MAX_SAVE_PATH_LENGTH> file("%s/%s.bin", directory, Comp::get_class_name());
					std::ofstream f(file.c_str(), std::ofstream::binary);

					if (!f)
						return false;

					detail::ReadLock l(_mutex);

					const size_t size = chunks.size();
					f.write((const char *)&size, sizeof(size));

					for (const auto & chunk : chunks) {
						const bool empty = chunk.empty();
						f.write((const char *)&empty, sizeof(empty));
						if (!empty)
							f.write((const char *)chunk.data(), KENGINE_COMPONENT_CHUNK_SIZE * sizeof(Comp));
					}
					return true;
				}
				return false;
			}

			bool load(const char * directory) final {
				if constexpr (putils::has_member_get_class_name<Comp>::value && !std::is_base_of<kengine::not_serializable, Comp>::value) {
					putils::string<KENGINE_MAX_SAVE_PATH_LENGTH> file("%s/%s.bin", directory, Comp::get_class_name());
					std::ifstream f(file.c_str(), std::ifstream::binary);
					if (!f)
						return false;

					size_t size;
					f.read((char *)&size, sizeof(size));

					detail::WriteLock l(_mutex);
					chunks.clear();
					for (size_t i = 0; i < size; ++i) {
						chunks.emplace_back(0);
						bool empty;
						f.read((char *)&empty, sizeof(empty));
						if (!empty) {
							chunks.back().resize(KENGINE_COMPONENT_CHUNK_SIZE);
							f.read((char *)chunks.back().data(), KENGINE_COMPONENT_CHUNK_SIZE * sizeof(Comp));
						}
					}
					return true;
				}
				return false;
			}
		};

	public:
		static Comp & get(size_t id) { static auto & meta = metadata();
			detail::ReadLock r(meta._mutex);

			if (id >= meta.chunks.size() * KENGINE_COMPONENT_CHUNK_SIZE) {
				r.unlock(); { // Unlock read so we can get write
					detail::WriteLock l(meta._mutex);
					while (id >= meta.chunks.size() * KENGINE_COMPONENT_CHUNK_SIZE)
						meta.chunks.emplace_back(0);
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
				const auto typeIndex = pmeta::type<Comp>::index;

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
#ifndef NDEBUG
				std::cout << ptr->id << ' ' << ptr->funcs.name << '\n';
#endif
				return ptr;
			}();

			return *ret;
		}

		static const char * getName() {
			if constexpr (putils::has_member_get_class_name<Comp>::value)
				return Comp::get_class_name();
			else
				return typeid(Comp).name();
		}
	};
}
