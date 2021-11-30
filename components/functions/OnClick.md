# [OnClick](OnClick.hpp)

`Function Component` that gets called when the parent `Entity` is clicked.

## Prototype

```cpp
void (int button);
```

### Parameters

* `button`: mouse button that was clicked

## Usage

The [OnClickSystem](../../systems/onclick/OnClickSystem.md) will call this `Component` on the `Entity` found in the clicked pixel (obtained through an [InputComponent](../data/InputComponent.md)).