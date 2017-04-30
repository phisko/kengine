# EntityManager

Manages [GameObjects](GameObject.md), [Components](Component.md) and [Systems](System.md).

An `EntityManager` is also a [Mediator](putils/mediator/README.md), managing communication for `Systems`.

### Members

##### Constructor

```
EntityManager(std::unique_ptr<EntityFactory> &&factory = nullptr);
```

An `EntityManager` can be constructed with an `EntityFactory`, which will be used to `create` entities.

##### execute

```
void execute();
```

Calls `execute()` for each `System`.

##### createSystem

```
template<typename T>
void createSystem(auto &&...args)
```

Creates a new `System` of type `T` and registers it.

##### addSystem

```
void addSystem(std::unique_ptr<ISystem> &&system);
```

Registers an already existing `System`.

##### createEntity

```
GameObject &createEntity(const std::string &type, const std::string &name,
        const std::function<void(GameObject &)> &postCreate = nullptr)
```

Asks the underlying `EntityFactory` to create an entity of type `type`, with the given `name`.

Once creation is complete, calls `postCreate` on the entity.

```
template<class GO, typename = std::enable_if_t<std::is_base_of<GameObject, GO>::value>>
GO &createEntity(std::string const &name,
        const std::function<void(GameObject &)> &postCreate = nullptr,
        auto &&... params) noexcept
```

Creates a new entity of type `GO` by giving it `params` as constructor arguments.

Once creation is complete, calls `postCreate` on the entity.

##### removeEntity

```
void removeEntity(kengine::GameObject &go);
void removeEntity(std::string const &name);
```

##### getEntity

```
GameObject &getEntity(const std::string &name);
```

##### attachComponent

```
template<class CT>
CT &attachComponent(GameObject &parent, auto &&... params);
```

Creates a new `Component` of type `CT`, passing it `params` as constructor arguments, and attaches it to `parent`.

##### detachComponent

```
void detachComponent(GameObject &go, const IComponent &comp);
```

Removes `comp` from `go`.
