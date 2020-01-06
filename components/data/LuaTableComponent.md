# [LuaTableComponent](LuaTableComponent.hpp)

## Specs

`Component` that holds a lua table that can be used to store arbitrary information in the `Entity`.

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Not serializable (contains pointers and internal lua state)
* Not processed by any system (used to store arbitrary user data)

## Members

### table

```cpp
sol::table table;
```

Lua scripts can set this value to anything they desire.