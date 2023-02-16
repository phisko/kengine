# [on_click](on_click.hpp)

`Function component` called when the entity is clicked.

## Prototype

```cpp
void (int button);
```

### Parameters

* `button`: mouse button that was clicked

## Usage

The [on_click system](../systems/on_click.md) will call this `function component` for the entity found in the clicked pixel.