# [ScriptLanguageHelper](ScriptLanguageHelper.hpp)

Set of helper functions meant to be used by language-specific scripting systems (e.g. the [LuaSystem](LuaSystem.md) or the [PySystem](PySystem.md)).

## Members

All these functions are only meant for internal use by the scripting systems.

### init

```cpp
template<typename Func, typename Func2>
void init(EntityManager & em, Func && registerFunction, Func2 && registerType);
```

Calls `registerFunction` for the following functions:

* `createEntity(function postCreate)`
* `removeEntity(Entity e)`
* `removeEntityById(Entity::ID id)`
* `getEntity(Entity::ID id)`
* `stopRunning()`

Calls `registerType` with `putils::meta::type<Entity>` as parameter.

### registerComponent

```cpp
template<typename T, typename Func>
void registerComponent(Func && registerEntityMember);
```

Calls `registerEntityMember` to register the following functions:

* `getT()` (e.g. `getGraphicsComponent()`)
* `hasT()` (e.g. `hasGraphicsComponent()`)
* `attachT()` (e.g. `attachGraphicsComponent()`)
* `detachT()` (e.g. `detachGraphicsComponent()`)

This lets scripts perform any operation on `Entities` if the necessary types are registered. Client code can either give full access to scripts by registering all its types (and therefore having a fully extensible game that can be developed almost entirely in a scripting language), or only register a small set of types and/or members, to restrict what modders can do.