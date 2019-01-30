#pragma once

#ifndef NDEBUG
#include <iostream>
#endif

#include <fstream>
#include <cstddef>
#include <unordered_map>
#include <memory>
#include <vector>
#include "meta/type.hpp"
#include "reflection/Reflectible.hpp"
#include "not_serializable.hpp"
#include "string.hpp"

namespace kengine {
	namespace detail {
		static constexpr size_t INVALID = (size_t)-1;

		struct MetadataBase {
			virtual ~MetadataBase() = default;
			virtual bool save() const = 0;
			virtual bool load() = 0;
			virtual size_t getId() const = 0;
		};
		using GlobalCompMap = std::unordered_map<pmeta::type_index, std::unique_ptr<MetadataBase>>;
		extern GlobalCompMap * components;
	}

	template<typename Comp>
	class Component {
	private:
		struct Metadata : detail::MetadataBase {
			std::vector<Comp> array;
			size_t id = detail::INVALID;

			bool save() const final {
				if constexpr (putils::is_reflectible<Comp>::value && !std::is_base_of<kengine::not_serializable, Comp>::value) {
					putils::string<64> file("%s.bin", Comp::get_class_name());
					std::ofstream f(file.c_str());

					if (!f)
						return false;

					const auto size = array.size();
					f.write((const char *)&size, sizeof(size));
					f.write((const char *)array.data(), size * sizeof(Comp));
					return true;
				}
				return false;
			}

			bool load() final {
				if constexpr (putils::has_member_get_class_name<Comp>::value && !std::is_base_of<kengine::not_serializable, Comp>::value) {
					putils::string<64> file("%s.bin", Comp::get_class_name());
					std::ifstream f(file.c_str());
					if (!f)
						return false;

					size_t size;
					f.read((char *)&size, sizeof(size));
					array.resize(size);
					f.read((char *)array.data(), size * sizeof(Comp));
					return true;
				}
				return false;
			}

			size_t getId() const final { return id;  }
		};

	public:
		static Comp & get(size_t id) { static auto & meta = metadata();
			while (id >= meta.array.size())
				meta.array.resize(meta.array.size() * 2);
			return meta.array[id];
		}

		static size_t id() {
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

#ifndef NDEBUG
					if constexpr (putils::is_reflectible<Comp>::value)
						std::cout << ret->id << ' ' << Comp::get_class_name() << '\n';
#endif
				}
			}

			return *ret;
		}
	};
}
