# [LuaSystem](LuaSystem.hpp)

`System` that executes lua scripts, either at a global scale or attached to a `GameObject` through a [LuaComponent](../components/LuaComponent.md).

### Members

##### Constructor

The constructor automatically defines the following functions for lua scripts:

* `getGameObjects()`: returns all the `GameObjects` currently in existence.
* `createEntity(string type, string name)`
* `removeEntity(string name)`
* `getEntity(string name)`
* `hasEntity(string name)`
* `getDeltaTime()`
* `getFixedDeltaTime()`
* `getDeltaFrames()`
* `stopRunning()`

The [GameObject](../../GameObject.md) type is also registered.

Scripts attached to `GameObjects` can use the `self` global variable to access the `GameObject` they are attached to.

```
/!\
```
Calling `createEntity` or `removeEntity` from a script attached to a `GameObject` will result in the entity being created or remove **only at the end of the current frame**. This is necessary to avoid memory corruption errors, as the newly created entities could invalidate iterators in the collection of entities being currently iterated on by the [LuaSystem](../systems/LuaSystem.md).

If a *"scripts"* directory is found, `addScriptDirectory("scripts")` is called.

##### registerType

```cpp
template<typename T>
void registerType() noexcept;
```
Registers the [Reflectible](https://github.com/phiste/putils/blob/master/reflection/README.md) type `T` with the underlying lua state.

This allows lua scripts to access any attribute or method in `T`'s `Reflectible` API.

This also defines the following function for lua scripts:
* `sendT(T packet)` (e.g. `sendLog(Log packet)`): sends `packet` as a datapacket to all `Systems`

If `T` is a `Component`, the following member functions is added to the `GameObject` lua type for lua scripts:
* `getT()` (e.g. `getMetaComponent()`)
* `hasT()` (e.g. `hasMetaComponent()`)
* `attachT()` (e.g. `attachMetaComponent()`)
* `detachT()` (e.g. `detachMetaComponent()`)

A global `getGameObjectsWithT()` is also defined, that returns all the `GameObjects` with a `T` component.

This allows lua scripts to perform any operation on `GameObjects` if the necessary types are registered. Client code can either give full access to lua scripts by registering all its types (and therefore having a fully extensible game that can be developed almost entirely in lua), or only register a small set of types and/or members, to restrict what modders can do.

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
Adds `dir` as one of the directories in which to execute lua scripts. Each frame, all the files in `dir` will be interpreted as lua scripts and executed.
