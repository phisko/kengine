# [ExtensibleFactory](EntityFactory.hpp)

A default implementation for `EntityFactory`, which lets users register their types at runtime.

### Members

##### Creator

```cpp
using Creator = std::function<std::unique_ptr<GameObject>(std::string_view name)>;
```
A `Creator` is a function that will be called to create a given type.

##### registerType

```cpp
template<typename T>
void registerType();
```
Assuming `T` is a [putils::Reflectible](https://github.com/phiste/putils/blob/master/reflection/README.md), registers a creator for `T` under its name.

##### registerTypes

```cpp
template<typename ...Types>
void registerTypes();
```
Calls `registerType` for each `T` in `Types`.

##### addType

```cpp
void addType(std::string_view type, const Creator &creator);
```
Registers `creator` as the function to be called to create `type`.

##### make

```cpp
std::unique_ptr<GameObject> make(std::string_view type, std::string_view name) final;
```
Uses the `Creator` registered for `type` to create a `GameObject`.
