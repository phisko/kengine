# [SystemManager](SystemManager.hpp)

Manages `Systems`, and serves as a [Mediator](https://github.com/phisko/putils/blob/master/mediator/README.md) for them.

### Members

##### execute

```cpp
void execute() const;
```
Calls the `execute` function of each `System`.

##### createSystem

```cpp
template<typename T, typename ...Args>
void createSystem(Args &&...args);
```
Creates and registers a new `System` of type `T`, passing it `args` as constructor parameters.

##### addSystem

```cpp
void addSystem(std::unique_ptr<ISystem> &&system);
```
Adds a pre-existing `System`.

##### loadSystems

```cpp
template<typename ...Systems>
void loadSystems(std::string_view pluginDir = "", std::string_view creatorFunction = "getSystem", bool pluginsFirst = false);
```
Calls `createSystem` for each type in `Systems`, passing them an `EntityManager &` as their only constructor parameter.
 
If `pluginDir` is provided, for each shared library (.so or .dll) found in `pluginDir`, calls the `creatorFunction` function from it, which should take an `EntityManager &` as parameter and return an `ISystem *`.

`pluginsFirst` makes it possible to specify whether plugins should be loaded before or after the template parameters.

##### getSystem

```cpp
template<typename T>
T &getSystem();
```
Gets the `System` of type `T`.

##### runTask
```cpp
template<typename F> // F: void()
void runTask(F && f);
```

Runs `f` in a thread pool.

##### completeTasks

```cpp
void completeTasks();
```

Waits for the thread pool to complete its tasks.

##### runAfterSystem

```cpp
void runAfterSystem(const std::function<void()> & func);
```

Lets `Systems` provide a function to be called after `execute` returns. This can be useful to call functions which could invalidate a state (such as `load`).