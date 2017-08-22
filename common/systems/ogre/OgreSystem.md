# [OgreSystem](OgreSystem.hpp)

`System` that displays entities in an OGRE window. The OGRE library must be installed separately.

`OgreSystem` is provided as a plugin. If the `KENGINE_OGRE` option is set to `TRUE` in the project's `CMakeLists.txt`, the `OgreSystem` will be automatically built into a shared library, in the `CMAKE_LIBRARY_OUTPUT_DIRECTORY`. Client code must then load the plugin (with [loadSystems](../../../SystemManager.md) for instance) at runtime.

### Using OgreSystem

##### 'Normal' objects

`OgreSystem` reads the mesh to be used for a `GameObject` from its [MetaComponent](../../components/MetaComponent.md)'s `appearance` property.

If `appearance` is *"light"*, `OgreSystem` will spawn a `PointLight` instead of a mesh.

`OgreSystem` expects `GameObjects` to have a [TransformComponent3d](../../components/TransformComponent.md) component, to specify its size and position.

```
/!\ That 3d is important! TransformComponent2d, 2i, 3i, 3f... Will not be detected!
```

##### Cameras

If a `GameObject` is found to have a [CameraComponent](../../components/CameraComponent.hpp), a camera will be added to the scene. The camera's position and orientation will be updated accordingly to the `CameraComponent`.

##### GUI

If a `GameObject` is found to have a [GUIComponent](../../components/GUIComponent.md), it will be rendered as text using the information held in that `GUIComponent`.

##### Input


If a [kengine::LuaSystem](../../systems/LuaSystem.md) is found when the `SfSystem` is constructed, the following lua functions are registered:

* `setKeyHandler(std::function<void(SDL_Scancode)> onPress, std::function<void(SDL_Scancode)> onRelease)`: sets the key handler for all keys
* `setMouseButtonHandler(std::function<void(char button, int x, int y)> onPress, std::function<void(char button)> onRelease)`: sets the button handler for all keys
* `setMouseMovedHandler(std::function<void(int x, int y)> func)`: sets the mouse move handler for all keys

