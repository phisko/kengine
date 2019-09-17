# [ScriptSystem](ScriptSystem.hpp)

`System` meant to be inherited in order to execute scripts for a given language (e.g. [lua](common/systems/LuaSystem.md) or [python](common/systems/PySystem.md)), once they are attached to an `Entity` through a `Component` (e.g. a [LuaComponent](common/components/LuaComponent.md)).

### Members

##### Constructor

The constructor automatically defines the following functions for scripts:

* `getEntities()`: returns all the `Entities` currently in existence.
* `createEntity(function postCreate)`
* `removeEntity(Entity e)`
* `removeEntityById(Entity::ID id)`
* `getEntity(Entity::ID id)`
* `getDeltaTime()`
* `getFixedDeltaTime()`
* `getDeltaFrames()`
* `stopRunning()`
* `pause()`
* `resume()`
* `isPaused()`
* `setSpeed()`
* `getSpeed()`

The [Entity](Entity.md) type is also registered.

Scripts attached to `Entities` can use the `self` global variable to access the `Entity` they are attached to.

##### registerType

```cpp
template<typename T>
void registerType() noexcept;
```
Registers the [Reflectible](https://github.com/phiste/putils/blob/master/reflection/README.md) type `T` with the underlying language.

This allows scripts to access any attribute or method in `T`'s `Reflectible` API.

This also defines the following function for scripts:
* `sendT(T packet)` (e.g. `sendLog(Log packet)`): sends `packet` as a datapacket to all `Systems`

If `T` is a `Component`, the following member functions is added to the `Entity` type for scripts:
* `getT()` (e.g. `getGraphicsComponent()`)
* `hasT()` (e.g. `hasGraphicsComponent()`)

A global `getEntitiesWithT()` is also defined, that returns all the `Entities` with a `T` component.

This lets scripts perform any operation on `Entities` if the necessary types are registered. Client code can either give full access to scripts by registering all its types (and therefore having a fully extensible game that can be developed almost entirely in a scripting language), or only register a small set of types and/or members, to restrict what modders can do.

##### registerTypes

```cpp
template<typename ...Types>
void registerTypes() noexcept;
```

For each type in `Types`, call `registerType` for it.
