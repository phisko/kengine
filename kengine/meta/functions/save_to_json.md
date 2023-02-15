# [save_to_json](save_to_json.hpp)

`Meta component` that serializes the parent component into a [JSON](https://github.com/nlohmann/json) object. 

## Prototype

```cpp
nlohmann::json (entt::const_handle);
```

### Parameters

* `e`: entity whose component we want to serialize

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to parse.

A [standard implementation](../helpers/impl/save_to_json.md) is provided.

Note that the implementation is only a sample, and users may freely replace it with any other implementation they desire.

