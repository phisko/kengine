# [ScriptSystem](ScriptSystem.hpp)

`System` meant to be inherited in order to execute scripts for a given language (e.g. [lua](common/systems/LuaSystem.md) or [python](common/systems/PySystem.md)), either at a global scale or attached to a `GameObject` through a `Component` (e.g. a [LuaComponent](common/components/LuaComponent.md)).

### Members

##### Constructor

The constructor automatically defines the following functions for scripts:

* `getGameObjects()`: returns all the `GameObjects` currently in existence.
* `createEntity(string type, string name, function postCreate)`
* `createNamelessEntity(string type, function postCreate)`
* `removeEntity(GameObject go)`
* `removeEntityByName(string name)`
* `getEntity(string name)`
* `hasEntity(string name)`
* `getDeltaTime()`
* `getFixedDeltaTime()`
* `getDeltaFrames()`
* `stopRunning()`
* `pause()`
* `resume()`
* `isPaused()`
* `setSpeed()`
* `getSpeed()`

The [GameObject](GameObject.md) type is also registered.

Scripts attached to `GameObjects` can use the `self` global variable to access the `GameObject` they are attached to.

##### registerType

```cpp
template<typename T>
void registerType() noexcept;
```
Registers the [Reflectible](https://github.com/phiste/putils/blob/master/reflection/README.md) type `T` with the underlying lua state.

This allows scripts to access any attribute or method in `T`'s `Reflectible` API.

This also defines the following function for scripts:
* `sendT(T packet)` (e.g. `sendLog(Log packet)`): sends `packet` as a datapacket to all `Systems`

If `T` is a `Component`, the following member functions is added to the `GameObject` type for scripts:
* `getT()` (e.g. `getGraphicsComponent()`)
* `hasT()` (e.g. `hasGraphicsComponent()`)
* `attachT()` (e.g. `attachGraphicsComponent()`)
* `detachT()` (e.g. `detachGraphicsComponent()`)

A global `getGameObjectsWithT()` is also defined, that returns all the `GameObjects` with a `T` component.

This lets scripts perform any operation on `GameObjects` if the necessary types are registered. Client code can either give full access to scripts by registering all its types (and therefore having a fully extensible game that can be developed almost entirely in a scripting language), or only register a small set of types and/or members, to restrict what modders can do.

##### registerTypes

```cpp
template<typename ...Types>
void registerTypes() noexcept;
```

For each type in `Types`, call `registerType` for it.

##### addScriptDirectory

```cpp
void addScriptDirectory(std::string_view dir) noexcept;
```
Adds `dir` as one of the directories in which to execute lua scripts. Each frame, all the files in `dir` will be interpreted as scripts and executed.
