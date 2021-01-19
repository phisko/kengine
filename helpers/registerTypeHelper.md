# [registerTypeHelper](registerTypeHelper.hpp)

Helper functions to quickly register all currently implemented type utilities (`meta Components`, bindings for scripting languages) for a set of types. These functions will not compile if all the functionalities they apply to are not activated (e.g. Lua bindings, Python bindings...)

## Members

### registerTypes

```cpp
template<typename ... Types>
void registerTypes() noexcept;
```

Registers a set of non-component types. For each `T` in `Types`:

* calls [`lua::registerType<T>()`](luaHelper.md)
* calls [`python::registerType<T>()`](pythonHelper.md)
* calls `registerTypes<U>()` for each `U` in `T`'s [used types](https://github.com/phisko/putils/reflection.md)

### registerComponents

```cpp
template<typename ... Comps>
void registerComponents() noexcept;
```

Registers a set of component types:

* calls `registerTypes<Comps...>()`
* adds a [NameComponent](../components/data/NameComponent.md) to the `type Entities` for `Comps` with their class names
* calls [`registerAttachTo<Comps...>()`](meta/registerAttachTo.md)
* calls [`registerDetachFrom<Comps...>()`](meta/registerDetachFrom.md)
* calls [`registerDisplayImGui<Comps...>()`](meta/registerDisplayImGui.md)
* calls [`registerEditImGui<Comps...>()`](meta/registerEditImGui.md)
* calls [`registerForEachAttribute<Comps...>()`](meta/registerForEachAttribute.md)
* calls [`registerForEachEntity<Comps...>()`](meta/registerForEachEntity.md)
* calls [`registerHas<Comps...>()`](meta/registerHas.md)
* calls [`registerLoadFromJSON<Comps...>()`](meta/registerLoadFromJSON.md)
* calls [`registerMatchString<Comps...>()`](meta/registerMatchString.md)
* calls [`registerSaveToJSON<Comps...>()`](meta/registerSaveToJSON.md)

### registerFunction

```cpp
template<typename F>
void registerFunction(const char * name, F && func) noexcept;
```

Registers a function with all scripting languages (Lua and Python).
