# [InputComponent](InputComponent.hpp)

`Component` that lets entities receive input events.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Not serializable

### Members

##### onKey

```cpp
putils::function<void(int keycode, bool pressed), KENGINE_INPUT_FUNCTION_SIZE> onKey;
```

##### onMouseMove

```cpp
putils::function<void(float x, float y, float xrel, float yrel), KENGINE_INPUT_FUNCTION_SIZE> onMouseMove;
```

##### onMouseButton

```cpp
putils::function<void(int button, float x, float y, bool pressed), KENGINE_INPUT_FUNCTION_SIZE> onMouseButton;
```

##### onMouseWheel

```cpp
putils::function<void(float delta, float x, float y, KENGINE_INPUT_FUNCTION_SIZE)> onMouseWheel;
```

The maximum size for these functors defaults to 64 and can be adjusted by defining the `KENGINE_INPUT_FUNCTION_SIZE` macro.