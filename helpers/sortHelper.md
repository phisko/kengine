# [sortHelper](sortHelper.hpp)

Helper functions to sort sets of `Entities`.

## Members

### getSortedEntities

```cpp
template<typename ... Comps, typename Pred>
auto getSortedEntities(Pred && pred) noexcept;

template<size_t MaxCount, typename ... Comps, typename Pred>
auto getSortedEntities(Pred && pred) noexcept;
```

Returns a list of all `Entities` with the `Comps` components, sorted according to `pred`.

These are returned under the form of a container of `tuple<Entity, Comps * ...>` instead of the usual `tuple<Entity, Comps & ...>` returned by `entities.with`, so you need to dereference the component pointers.

`Pred` is a functor with the `bool(tuple<Entity, Comps * ...> lhs, tuple<Entity, Comps * ...> rhs)` signature, which returns `true` if `lhs` should appear before `rhs`.

If `MaxCount` is provided, the function returns a fixed-size `putils::vector` instead of `std::vector`, avoiding the heap allocation.

#### Example

```cpp
const auto sorted = sortHelper::getSortedEntities<64, NameComponent, TransformComponent>(
    [](const auto & lhs, const auto & rhs) {
        // lhs and rhs are std::tuple<Entity, NameComponent *, TransformComponent *>;
        // std::get<1> returns a NameComponent *
        return strcmp(std::get<1>(lhs)->name, std::get<1>(rhs)->name) < 0;
    }
);

for (const auto & [e, name, transform] : sorted) {
    std::cout << name->name << std::endl;
    std::cout << transform->boundingBox << std::endl;
}
```

### getNameSortedEntities

```cpp
template<typename ... Comps>
auto getNameSortedEntities() noexcept;

template<size_t MaxCount, typename ... Comps>
auto getNameSortedEntities() noexcept;
```

Returns a list of all `Entities` with the `Comps` components and a `NameComponent`, sorted in alphabetical order according to their name.

These are returned under the form of a container of `tuple<Entity, NameComponent *, Comps * ...>`.

If `MaxCount` is provided, the function returns a fixed-size `putils::vector` instead of `std::vector`, avoiding the heap allocation.

#### Example

```cpp
const auto sorted = sortHelper::getNameSortedEntities<64, TransformComponent>();

for (const auto & [e, name, transform] : sorted) {
    std::cout << name->name << std::endl;
    std::cout << transform->boundingBox << std::endl;
}
```
