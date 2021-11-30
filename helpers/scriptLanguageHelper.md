# [scriptLanguageHelper](scriptLanguageHelper.hpp)

Set of helper functions meant to be used by language-specific scripting systems (e.g. the [LuaSystem](LuaSystem.md) or the [PythonSystem](PythonSystem.md)).

## Members

All these functions are only meant for internal use by the scripting systems.

### init

```cpp
template<typename Func, typename Func2>
void init(Func && registerFunction, Func2 && registerType) noexcept;
```

Calls `registerFunction` for the following functions:

* `createEntity(function postCreate)`
* `removeEntity(Entity e)`
* `removeEntityById(EntityID id)`
* `getEntity(EntityID id)`
* `forEachEntity(function f)`
* `stopRunning()`

Calls `registerType` with `putils::meta::type<Entity>` as parameter.

### registerComponent

```cpp
template<typename T, typename Func>
void registerComponent(Func && registerEntityMember) noexcept;
```

Calls `registerEntityMember` to register the following functions:

* `getT()` (e.g. `getGraphicsComponent()`)
* `tryGetT()` (e.g. `tryGetGraphicsComponent()`)
* `hasT()` (e.g. `hasGraphicsComponent()`)
* `attachT()` (e.g. `attachGraphicsComponent()`)
* `detachT()` (e.g. `detachGraphicsComponent()`)
* `forEachEntityWithT(function f)` (e.g. `forEachEntityWithTransformComponent()`)

This lets scripts perform any operation on `Entities` if the necessary types are registered. Client code can either give full access to scripts by registering all its types (and therefore having a fully extensible game that can be developed almost entirely in a scripting language), or only register a small set of types and/or members, to restrict what modders can do.
