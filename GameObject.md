# GameObject

Represents an in-game entity. `GameObjects` do not hold any data themselves, but instead contain [Components](Component.md).

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
