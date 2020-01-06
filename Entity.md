# [Entity](Entity.hpp)

Represents an in-game entity. `Entities` do not hold any data themselves, but instead contain `Components`. Any type can be used as a `Component`.

Depending on the context (e.g. when iterating over entities) you may receive `EntityView` objects instead of `Entities`. An `EntityView` is simply the `const` version of an `Entity`.

## Members

### attach

```cpp
template<typename T>
T & attach();
```
Creates and attaches a new `Component` of type `T`.

### operator+=

```cpp
template<typename T>
Entity & operator+=(T && comp);
```
Attaches a new `Component` of type `T` and assigns `comp` to it.

### detach

```cpp
template<typename T>
void detach();
```

### get

```cpp
template<class T>
T & get() const;
```

Returns the `Component` of type `T` attached to this.
`asserts` if there is no such component.

### has

```cpp
template<typename T>
bool has() const;
```

Returns whether a `Component` of type `T` is attached to this.
