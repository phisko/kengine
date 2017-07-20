# SfSystem

`System` that displays entities in an SFML window.

`SfSystem` is the only sample provided as a plugin. If the `KENGINE_SFML` option is set to `TRUE` in the project's `CMakeLists.txt`, the `SfSystem` will be automatically built into a shared library, in the `CMAKE_LIBRARY_OUTPUT_DIRECTORY`. Client code must then load the plugin (with [loadSystems](../../../SystemManager.md) for instance) at runtime.

### Using SfSystem

`SfSystem` reads the resource file to be used for a `GameObject` from its [MetaComponent](../../components/MetaComponent.md)'s `appearance` property.

If `appearance` was previously registered as an abstract appearance through a [RegisterAppearance](../../packets/RegisterAppearance.hpp) datapacket, the resource file that was associated to it is loaded instead.

`SfSystem` expects `GameObjects` to have a [TransformComponent3d](../../components/TransformComponent.md) component, to specify its size and position. The position's `z` property defines the "height" at which objects will be rendered.

```
/!\ That 3d is important! TransformComponent2d, 2i, 3i, 3f... Will not be detected!
```

### Configuration

##### sf-config.json

`SfSystem` loads its configuration from `sf-config.json`, which is automatically copied into the `CMAKE_RUNTIME_OUTPUT_DIRECTORY`.

This file should contain a single JSON object, with `windowSize`, `tileSize` and `fullScreen` properties.

`windowSize` and `tileSize` should each be JSON objects with an `x` and `y` field, defining the window resolution and the number of pixels that should be used for a `GameObject` with 1 as its `TransformComponent3d`'s `size`.

`fullScreen` is a boolean that defines whether the window should be rendered in fullscreen or not.

##### Example

```
{
    windowSize: {
        x: 1280,
        y: 720
    }
    tileSize: {
        x: 16,
        y: 16
    }
    fullScreen: false
}
```


