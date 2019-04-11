# [InputComponent](InputComponent.hpp)

`Component` that lets entities receive input events.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Not serializable

### Members

##### onKey

```cpp
std::function<void(int keycode, bool pressed)> onKey;
```

##### onMouseMove

```cpp
std::function<void(float x, float y)> onMouseMove;
```

##### onMouseButton

```cpp
std::function<void(int button, float x, float y, bool pressed)> onMouseButton;
```

##### onMouseWheel

```cpp
std::function<void(float delta, float x, float y)> onMouseWheel;
```