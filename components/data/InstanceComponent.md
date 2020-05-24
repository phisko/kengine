# [InstanceComponent](InstanceComponent.hpp)

`Component` indicating that the parent `Entity` is an instance of another `Entity`, called the `model Entity`. This helps set up a Flyweight design pattern, and is notably used in graphics systems. In the case of graphics systems, `model Entities` have a [ModelComponent](ModelComponent.md) and all the other `Components` holding graphics-related information, which is the same for all instances. `Entities` which need to be drawn with the same model all have an `InstanceComponent` pointing to the same `model Entity`.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD), although the [model](#model) attribute will be invalidated

### Members

### model

```cpp
Entity::ID model = Entity::INVALID_ID;
```

id of the `model Entity`.