#pragma once

#include <iostream>
#include <cstddef>
#include <unordered_map>
#include <memory>
#include <vector>
#include "meta/type.hpp"
#include "reflection/Reflectible.hpp"

namespace kengine {
	namespace detail {
		static constexpr size_t INVALID = (size_t)-1;

		struct MetadataBase {
			virtual ~MetadataBase() = default;
		};
		using GlobalCompMap = std::unordered_map<pmeta::type_index, std::unique_ptr<MetadataBase>>;
		static inline GlobalCompMap * components = nullptr;
	}

	template<typename Comp>
	class Component {
	private:
		struct Metadata : detail::MetadataBase {
			std::vector<Comp> array;
			size_t id = detail::INVALID;
		};

	public:
		static inline Comp & get(size_t id) { auto & meta = metadata();
			while (id >= meta.array.size())
				meta.array.resize(meta.array.size() * 2);
			return meta.array[id];
		}

		static inline size_t id() {
			static const size_t ret = metadata().id;
			return ret;
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
					ret->id = detail::components->size() - 1;
					ret->array.resize(64);

					if constexpr (putils::is_reflectible<Comp>::value)
						std::cout << ret->id << ' ' << Comp::get_class_name() << '\n';
				}
			}

			return *ret;
		}
	};
}
