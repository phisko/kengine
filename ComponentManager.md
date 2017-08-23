# [ComponentManager](ComponentManager.hpp)

Manages `Components` and their relationships to `GameObjects`.

### Members

##### attachComponent

```cpp
template<class CT, typename ...Params>
CT &attachComponent(GameObject &parent, Params &&... params) const noexcept;
```

Attaches a new component of type `CT` to `parent`, creating it with `params` as constructor parameters.

##### detachComponent

```cpp
void detachComponent(GameObject &go, const IComponent &comp) const;
```
Removes `comp` from `go`.

##### getParent

```cpp
const GameObject &getParent(const IComponent &comp) const;
```
Returns the `GameObject` to which `comp` is attached.

##### getGameObjects

```cpp
template<typename T>
const std::vector<GameObject *> &getGameObjects();
```
Returns all the `GameObjects` with a `T` attached to them. This is the main way for `Systems` to access `GameObjects`. The results for this function are pre-calculated, and its runtime cost is minimal.

```cpp
const std::vector<GameObject *> &getGameObjects() const;
```
Returns all `GameObjects`.
