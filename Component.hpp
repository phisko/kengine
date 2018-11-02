#pragma once

#include <cstddef>
#include <unordered_map>
#include <memory>
#include <vector>
#include "meta/type.hpp"

namespace kengine {
	namespace detail {
		static constexpr size_t INVALID = (size_t)-1;

		struct MetadataBase {};
		using GlobalCompMap = std::unordered_map<pmeta::type_index, std::unique_ptr<MetadataBase>>;
		static inline GlobalCompMap * components = nullptr;
	}

	template<typename Comp>
	class Component {
	private:
		struct Metadata : detail::MetadataBase {
			struct Link {
				size_t next;
				Comp comp;
			};

			size_t nextInit = 0;
			std::vector<Link> array;
			size_t next = 0;
			size_t id;

			std::string debugname = Comp::get_class_name();
		};

	public:
		static inline Comp & get(size_t id) { auto & meta = metadata();
			assert("Invalid component id" && id < meta.array.size());
			return meta.array[id].comp;
		}

		static inline size_t alloc() { auto & meta = metadata();
			assert("This should never happen" && meta.next <= meta.array.size());
			if (meta.next == meta.array.size()) {
				Comp c;
				meta.array.emplace_back(Metadata::Link{ meta.nextInit++, std::move(c) });
			}

			const auto id = meta.next;
			meta.next = meta.array[id].next;
			return id;
		}

		static inline void release(size_t id) { auto & meta = metadata();
			assert("Invalid component id" && id < meta.array.size());
			meta.array[id].next = meta.next;
			meta.next = id;
		}

		static inline size_t id() { const auto & meta = metadata();
			return meta.id;
		}

	private:
		static inline Metadata & metadata() {
			static Metadata * ret = nullptr;

			if (ret == nullptr) {
				const auto typeIndex = pmeta::type<Comp>::index;
				const auto it = detail::components->find(typeIndex);
				if (it != detail::components->end())
					ret = static_cast<Metadata *>(it->second.get());
				else {
					auto ptr = std::make_unique<Metadata>();
					ret = static_cast<Metadata *>(ptr.get());
					(*detail::components)[typeIndex] = std::move(ptr);
				}
				ret->id = detail::components->size() - 1;
			}

			return *ret;
		}
	};
}
