# [Entity](Entity.hpp)

Represents an in-game entity. `Entities` do not hold any data themselves, but instead contain `Components`. Any type can be used as a `Component`.

## Members

### attach

```cpp
template<typename T>
T & attach() noexcept;
```
Creates and attaches a new `Component` of type `T`.

### operator+=

```cpp
template<typename T>
Entity & operator+=(T && comp) noexcept;
```
Attaches a new `Component` of type `T` and assigns `comp` to it.

### detach

```cpp
template<typename T>
void detach() noexcept;
```

### get

```cpp
template<class T>
T & get() noexcept;
```

Returns the `Component` of type `T` attached to this.
`asserts` if there is no such component.

### has

```cpp
template<typename T>
bool has() const noexcept;
```

Returns whether a `Component` of type `T` is attached to this.

### tryGet

```cpp
template<typename T>
T * tryGet() noexcept;
```

Returns the `Component` of type `T` attached to this if there is one. If not, returns `nullptr`.