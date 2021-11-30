# [SaveToJSON](SaveToJSON.hpp)

`Meta Component` that serializes the parent `Component` into a [JSON](https://github.com/nlohmann/json) object. 

## Prototype

```cpp
putils::json (const Entity & e);
```

### Parameters

* `e`: `Entity` whose `Component` we want to serialize

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to parse.

A helper [registerSaveToJSON](../../helpers/meta/impl/registerSaveToJSON.md) function is provided that takes as a template parameter a set of `Component` types and implements the `SaveToJSON` `meta Component` for them.

Note that the implementation provided in `registerSaveToJSON` is only a sample, and users may freely replace it with any other implementation they desire.

