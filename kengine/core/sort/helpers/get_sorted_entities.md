# [get_sorted_entities](get_sorted_entities.hpp)

```cpp
template<typename ... Comps, typename Registry, typename Pred>
auto get_sorted_entities(Registry && r, Pred && pred) noexcept;

template<size_t MaxCount, typename ... Comps, typename Registry, typename Pred>
auto get_sorted_entities(Registry && r, Pred && pred) noexcept;
```

Returns a list of all entities with the `Comps` components, sorted according to `pred`.

These are returned under the form of a container of `tuple<entt::entity, Comps * ...>` instead of the usual `tuple<entt::entity, Comps & ...>` returned by `r.view<Comps>().each()`, so you need to dereference the component pointers.

`Pred` is a functor with the `bool(tuple<entt::entity, Comps * ...> lhs, tuple<entt::entity, Comps * ...> rhs)` signature, which returns `true` if `lhs` should appear before `rhs`.

If `MaxCount` is provided, the function returns a fixed-size `putils::vector` instead of `std::vector`, avoiding the heap allocation.

#### Example

```cpp
const auto sorted = core::sort::get_sorted_entities<64, core::name, core::transform>(
    r,
    [](const auto & lhs, const auto & rhs) {
        // lhs and rhs are std::tuple<Entity, core::name *, core::transform *>;
        // std::get<1> returns a core::name *
        return strcmp(std::get<1>(lhs)->name, std::get<1>(rhs)->name) < 0;
    }
);

for (const auto & [e, name, transform] : sorted) {
    std::cout << name->name << std::endl;
    std::cout << transform->bounding_box << std::endl;
}
```