# GameObject

Represents an in-game entity. `GameObjects` do not hold any data themselves, but instead contain [Components](Component.md).

Inherits from [putils::Reflectible](../../putils/reflection/Reflectible.md).

A `GameObject` is also a [Mediator](putils/mediator/README.md), managing communication for its `Components`.

### Members

##### Constructor

```
GameObject(std::string const &name);
```

Each `GameObject` must be given a unique name.

##### operator<<

```
friend std::ostream &operator<<(std::ostream &s, const kengine::GameObject &obj);
```

`GameObjects` can be serialized to a JSON-formatted string.

##### attachComponent

```
template<typename CT, typename ...Args>
CT &attachComponent(Args &&...args);
```
Creates and attaches a new `Component` of type `CT`.

```
template<typename CT>
CT &attachComponent(std::unique_ptr<CT> &&comp);
```

##### detachComponent

```
template<typename CT>
void detachComponent();
```

```
template<typename CT>
void detachComponent(const CT &comp);
```

##### getComponent

```
template<class CT>
CT &getComponent() const;
```

Returns the `Component` of type `CT` attached to this.

Throws an `std::out_of_range` if no `CT` is found.

##### hasComponent

```
template<typename CT>
bool hasComponent() const;
```

Returns whether a `Component` of type `CT` is attached to this.

##### getName

```
const std::string &getName() const;
```
