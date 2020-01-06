# [MatchString](MatchString.hpp)

`Meta Component` returning whether the parent `Component` contained in a given `Entity` matches a token string.

## Prototype

```cpp
bool (const Entity & e, const char * str);
```

### Return value

* `true` if `str` is found in the parent `Component`'s name
* `true` if `str` is found in any of the fields in `e`'s parent `Component`
* `false` otherwise

### Parameters

* `e`: `Entity` in which `str` should be searched
* `str`: token string to search for in `e`'s parent `Component`

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to parse.

A helper [registerComponentMatcher](../../helpers/RegisterComponentMatcher.md) function is provided that takes as a template parameter a `Component` type and implements the `MatchString` `meta Component` for it.

Note that the implementation provided in `registerComponentMatcher` is only a sample, and users may freely replace it with any other implementation they desire.