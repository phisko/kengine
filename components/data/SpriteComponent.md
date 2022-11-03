# [SpriteComponent](SpriteComponent.hpp)

```cpp
struct SpriteComponent2D : OnScreenComponent;
struct SpriteComponent3D;
```

"Tag" `Components` indicating that an `Entity`'s [GraphicsComponent](GraphicsComponent.md) points to a 2D or 3D sprite.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (empty for 3D, POD for 2D)
* Processed by graphics systems