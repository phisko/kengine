# [EntityManager](EntityManager.hpp)

Manages [GameObjects](GameObject.md), [Components](Component.md) and [Systems](System.md).

An `EntityManager` is also a [Mediator](https://github.com/phiste/putils/blob/master/mediator/README.md), managing communication for `Systems`.

### Base classes

An `EntityManager`'s role is split-up into three parts:

* the `EntityManager` itself, which simply manages entities
* a [ComponentManager](ComponentManager.md) base
* a [SystemManager](SystemManager.md) base

### Members

##### Constructor

```cpp
EntityManager(std::unique_ptr<EntityFactory> &&factory = nullptr);
```
An `EntityManager` can be constructed with an `EntityFactory`, which will be used to `create` entities.

##### createEntity

```cpp
GameObject &createEntity(std::string_view type, std::string name,
                        const std::function<void(GameObject &)> &postCreate = nullptr)
```

Asks the underlying `EntityFactory` to create an entity of type `type`, with the given `name`.

Once creation is complete, calls `postCreate` on the entity.

```cpp
template<class GO, typename = std::enable_if_t<std::is_base_of<GameObject, GO>::value>>
GO &createEntity(std::string const &name,
        const std::function<void(GameObject &)> &postCreate = nullptr,
        auto &&... params) noexcept
```

Creates a new entity of type `GO` by giving it `params` as constructor arguments.

Once creation is complete, calls `postCreate` on the entity.

##### removeEntity

```cpp
void removeEntity(kengine::GameObject &go);
void removeEntity(std::string_view name);
```

##### getEntity

```cpp
GameObject &getEntity(std::string_view name);
```

##### hasEntity

```cpp
bool hasEntity(std::string_view name) const noexcept;
```

##### addLink

```cpp
void addLink(const GameObject &parent, const GameObject &child);
```

Registers `parent` as `child`'s parent object. This process can be used by systems to store relations between `GameObjects`.

##### removeLink

```cpp
void removeLink(const GameObject &child);
```

##### getParent

```cpp
const GameObject &getParent(const GameObject &go) const;
```

##### getFactory

```cpp
template<typename T>
T &getFactory();
```
Returns the `EntityFactory` as a `T`.
