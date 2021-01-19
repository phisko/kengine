# [ForEachAttribute](ForEachAttribute.hpp)

`Meta Component` that iterates over all attributes of the parent `Component` for a given `Entity`.

## Prototype

```cpp
void(Entity & e, const AttributeIteratorFunc & func);

using AttributeIteratorSignature = void(const char * name, const void * member, putils::meta::type_index memberType);
```

### Parameters

* `e`: `Entity` whose attributes will be iterated over
* `func`: function that will be called for each attribute. Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_ATTRIBUTE_ITERATOR_FUNC_SIZE` macro

## Usage

It is up to the user to implement this `meta Component` for the `Component` types whose attributes they'd like to be able to iterate over.

A helper [registerForEachAttribute](../../helpers/meta/registerForEachAttribute.md) function is provided that takes as a template parameter a set of `Component` types and implements the `ForEachAttribute` `meta Component` for them.