# [ModelComponent](ModelComponent.hpp)

Used only by model loading systems.

`Component` that provides the `ID` of another `Entity` to be used as the owner's 3D model. Here's a little diagram to help:

```
[ Player entity ]          
GraphicsComponent: appearance = "player"
ModelComponent: modelInfo = 42

[ Unnamed entity, created by model loading system ]
id = 42
ModelLoaderComponent: function returning the model data used by `OpenGLSystem` to create meshes
```

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

### Members

##### modelInfo

```cpp
kengine::Entity::ID modelInfo;
```

`ID` of the `Entity` holding the model information to be used when rendering this `Entity`.