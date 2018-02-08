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

##### getGameObjects

```cpp
const std::vector<GameObject> &getGameObjects();
```

Returns all `GameObjects`.

```cpp
template<typename T>
const std::vector<GameObject> &getGameObjects<T>();
```

Returns all `GameObjects` with a `T` component.

##### pause

```cpp
void pause();
```

Used to pause the game by setting the `speed` to 0. Calling `isPaused` from a [System](System.md) will return `true`, and `time.getDeltaFrames()` will always return 0. This means that most systems do not have to take any special measures to adapt to the game being paused, as long as they use `time.getDeltaFrames()` to adapt their behavior. The [PhysicsSystem](common/systems/PhysicsSystem.md), for instance, multiplies the `GameObjects`' movement by `time.getDeltaFrames()` to accomodate for dropped frames, and will therefore automatically stop moving them when the game is paused.

##### resume

```cpp
void resume();
```

Used to resume the game by setting the `speed` to 1.

##### setSpeed

```cpp
void setSpeed(double speed);
```

Sets the game's speed to `speed`. This will impact the return value of `time.getDeltaFrames()` in `Systems`, letting them automatically adjust their behavior if they take framerate into account.

##### getSpeed

```cpp
double getSpeed() const;
```

Returns the game's speed.

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
