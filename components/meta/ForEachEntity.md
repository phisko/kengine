# [ForEachEntity/ForEachEntityWithout](ForEachEntity.hpp)

`Meta Components` that iterate over all `Entities` with/without the parent `Component`.

## Prototype

```cpp
void (const EntityIteratorFunc & func);
```

### Parameters

* `func`: function that will be called for each `Entity`. Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_ITERATOR_FUNC_SIZE` macro

## Usage

It is up to the user to implement these `meta Components` for the `Component` types they wish to be able to filter.

A helper [registerComponentIterator](../../helpers/RegisterComponentIterators.md) function is provided that takes as a template parameter a `Component` type and implements the `ForEachEntity` and `ForEachEntityWithout` `meta Components` for it.

