# [for_each_entity/for_each_entity_without](for_each_entity.hpp)

`Meta components` that iterate over all entities with/without the parent component.

## Prototype

```cpp
void (entt::registry & r, const entity_iterator_func & func);

using entity_iterator_signature = void(entt::handle);
```

### Parameters

* `r`: registry to iterate over
* `func`: function that will be called for each entity. Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_ENTITY_ITERATOR_FUNC_SIZE` macro.

## Usage

It is up to the user to implement these `meta components` for the component types they wish to be able to filter.

A helper [register_for_each_entity](../helpers/meta/impl/register_for_each_entity.md) function is provided that takes as a template parameter a set of component types and implements these `meta components` for them.