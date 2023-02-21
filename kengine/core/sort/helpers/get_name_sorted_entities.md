# [get_name_sorted_entities](get_name_sorted_entities.hpp)

```cpp
template<typename... Comps, typename Registry>
auto get_name_sorted_entities(Registry && r) noexcept;

template<size_t MaxCount, typename... Comps, typename Registry>
auto get_name_sorted_entities(Registry && r) noexcept;
```

Returns a list of all entities with the `Comps` components and a [name](../../data/name.md), sorted in alphabetical order according to their name.

These are returned under the form of a container of `tuple<Entity, core::name *, Comps * ...>`.

If `MaxCount` is provided, the function returns a fixed-size `putils::vector` instead of `std::vector`, avoiding the heap allocation.

#### Example

```cpp
const auto sorted = core::sort::get_name_sorted_entities<64, core::transform>(r);

for (const auto & [e, name, transform] : sorted) {
    std::cout << name->name << std::endl;
    std::cout << transform->bounding_box << std::endl;
}
```