# [LoadFromJSON](LoadFromJSON.hpp)

`Meta Component` that parses the parent `Component` from a [JSON](https://github.com/nlohmann/json) object and attaches it to a given `Entity`.

## Prototype

```cpp
void (const nlohmann::json & json, Entity & e);
```

### Parameters

* `json`: JSON object for `e`, NOT specifically for the parent `Component`
* `e`: `Entity` which the new `Component` should be attached to

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to parse.

A helper [registerLoadFromJSON](../../helpers/meta/impl/registerLoadFromJSON.md) function is provided that takes as a template parameter a `Component` type and implements the `LoadFromJSON` `meta Component` for it.

Note that the implementation provided in `registerLoadFromJSON` is only a sample, and users may freely replace it with any other implementation they desire.

