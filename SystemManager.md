# [SystemManager](SystemManager.hpp)

Manages `Systems`, and serves as a `Mediator` for them.

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
void loadSystems(std::string_view pluginDir = "plugins", std::string_view creatorFunction = "getSystem");
```
Calls `createSystem` for each type in `Systems`, passing them an `EntityManager &` as their only constructor parameter.
 
Then, for each shared library (.so or .dll) found in `pluginDir`, calls the `creatorFunction` function from it, which should take an `EntityManager &` as parameter and return an `ISystem *`.

##### getSystem

```cpp
template<typename T>
T &getSystem();
```
Gets the `System` of type `T`.
