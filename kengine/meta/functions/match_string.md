# [match_string](match_string.hpp)

`Meta component` returning whether the parent component contained in a given entity matches a token string.

## Prototype

```cpp
bool (entt::const_handle e, const char * str);
```

### Return value

* `true` if `str` is found in the parent component's name
* `true` if `str` is found in any of the fields in `e`'s parent component
* `false` otherwise

### Parameters

* `e`: entity in which `str` should be searched
* `str`: token string to search for in `e`'s parent component

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to parse.

A [standard implementation](../helpers/impl/match_string.md) is provided.

Note that the implementation is only a sample, and users may freely replace it with any other implementation they desire.