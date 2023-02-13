# [instance](instance.hpp)

Component marking the entity as an instance of another entity, called the `model entity`. This helps set up a Flyweight design pattern, and is notably used in graphics systems. In the case of graphics systems, `model entities` have a [model component](model.md) and all the other components holding shared graphics-related information. Entities which need to be drawn with the same model all have an `instance` component pointing to the same `model entity`.

### Members

### model

```cpp
entt::entity model = entt::null;
```

Points to the `model entity`.