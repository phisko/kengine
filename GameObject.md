# [GameObject](GameObject.hpp)

Represents an in-game entity. `GameObjects` do not hold any data themselves, but instead contain [Components](Component.md).

Inherits from [putils::Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md).

A `GameObject` is also a [Mediator](https://github.com/phiste/putils/blob/master/mediator/README.md), managing communication for its `Components`.

### Members

##### Constructor

```cpp
GameObject(std::string const &name);
```

Each `GameObject` must be given a unique name.

##### operator<<

```cpp
friend std::ostream &operator<<(std::ostream &s, const kengine::GameObject &obj);
```

`GameObjects` can be serialized to a JSON-formatted string.

##### attachComponent

```cpp
template<typename CT, typename ...Args>
CT &attachComponent(Args &&...args);
```
Creates and attaches a new `Component` of type `CT`.

```cpp
template<typename CT>
CT &attachComponent(std::unique_ptr<CT> &&comp);
```

##### detachComponent

```cpp
template<typename CT>
void detachComponent();
```

```cpp
template<typename CT>
void detachComponent(const CT &comp);
```

##### getComponent

```cpp
template<class CT>
CT &getComponent() const;
```

Returns the `Component` of type `CT` attached to this.

Throws an `std::out_of_range` if no `CT` is found.

##### hasComponent

```cpp
template<typename CT>
bool hasComponent() const;
```

Returns whether a `Component` of type `CT` is attached to this.

##### getName

```cpp
const std::string &getName() const;
```
