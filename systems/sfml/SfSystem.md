# [SfSystem](SfSystem.hpp)

`System` that displays entities in an SFML window.

`SfSystem` is provided as a plugin. If the `KENGINE_SFML` option is set to `TRUE` in the project's `CMakeLists.txt`, the `SfSystem` will be automatically built into a shared library, in the `CMAKE_LIBRARY_OUTPUT_DIRECTORY`. Client code must then load the plugin (with [loadSystems](../../../SystemManager.md) for instance) at runtime.

### Using SfSystem

##### 'Normal' objects

`SfSystem` reads the resource file to be used for a `GameObject` from its [GraphicsComponent](../../components/GraphicsComponent.md)'s `appearance` property.

If `appearance` was previously registered as an abstract appearance through a [RegisterAppearance](../../packets/RegisterAppearance.hpp) datapacket, the resource file that was associated to it is loaded instead.

`SfSystem` expects `GameObjects` to have a [TransformComponent3f](../../components/TransformComponent.md) component, to specify its size and position. The position's `z` property defines the "height" at which objects will be rendered.

If the `GraphicsComponent`'s `size` property's `x` or `y` fields are set to anything but 0, the `SfSystem` wil use these values instead of the `TransformComponent3f`'s to set the drawable's size.

A `GameObject`'s rotation is defined by its `TransformComponent3f`'s `yaw` property ADDED TO its `GraphicsComponent`'s yaw property. This lets you define a graphical yaw that you do not have to compensate throughout the rest of your code (as you simply work on the `TransformComponent`).

```
/!\ That 3f is important! TransformComponent2d, 2i, 3i, 3f... Will not be detected!
```

##### Cameras

A *"default"* camera is added upon system construction, meaning typical use does not require any action. For further configuration of the rendered areas, `CameraComponents3f` can be used.

If a `GameObject` is found to have a [CameraComponent3f](../../components/CameraComponent.hpp), a camera will be added to the scene.

 The `CameraComponent3f`'s `frustum` property defines the area to be rendered, whereas the `GameObject`'s `TransformComponent3f`'s `boundingBox` property defines the bounds of the viewport to be displayed. The `boundingBox.position.y` property defines the "height" at which the camera should be rendered (higher cameras appear in front of lower ones).

Refer to [the SFML website](https://www.sfml-dev.org/tutorials/2.0/graphics-view.php) for more information about viewports.

##### Input

User input can be handled by attaching [InputComponents](../../components/InputComponent.hpp) to `GameObjects`.

##### ImGui

The `SfSystem` can also render any [ImGui](https://github.com/ocornut/imgui) elements through functions provided by other systems. To render an `ImGui` window, simply add an [ImGuiComponent](../../components/ImGuiComponent.hpp) to a `GameObject`, providing a function that renders the window. For instance:

```c++
go.attachComponent<kengine::ImGuiComponent>(
	[](void * context) {
		kengine::packets::ImGuiDisplay::setupImGuiContext(context);
		ImGui::Begin("Window");
		if (ImGui::Button("Click me!"))
			std::cout << "Clicked" << std::endl;
		ImGui::End();
	}
);
```

The provided function must take a `void *` as parameter, and call `kengine::packets::ImGuiDisplay::setupImGuiContext()` before doing any actual rendering. This is necessary to recover `ImGui`'s global state from the `SfSystem`.

### Configuration

##### sf-config.json

`SfSystem` loads its configuration from `sf-config.json`, which is automatically copied into the `CMAKE_RUNTIME_OUTPUT_DIRECTORY`.

This file should contain a single JSON object, with `windowSize`, `tileSize` and `fullScreen` properties.

`windowSize` and `tileSize` should each be JSON objects with an `x` and `y` field, defining the window resolution and the number of pixels that should be used for a `GameObject` with 1 as its `TransformComponent3f`'s `size`.

`fullScreen` is a boolean that defines whether the window should be rendered in fullscreen or not.

##### Example

```
{
    "windowSize": {
        "x": 1280,
        "y": 720
    },
    "tileSize": {
        "x": 16,
        "y": 16
    },
    "fullScreen": false
}
```


