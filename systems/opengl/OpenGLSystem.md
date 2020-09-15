# [OpenGLSystem](OpenGLSystem.hpp)

System that renders `Entities` in an OpenGL render window.

The implementation (as well as the shaders) is still a WIP, and I am by no means an expert in computer graphics so this is not expected to be production ready. Heavy-load has not been tested, and bugs are to be expected. If you need to do heavy lifting with 3D graphics, it may be a good idea to implement a new `System` using a higher level library like Ogre3D.

## Shaders

Unless the `KENGINE_OPENGL_NO_DEFAULT_SHADERS` macro is defined, the following shaders are automatically created:

* [DirLight](DirLight.hpp), performs lighting for [DirLightComponents](../../components/data/LightComponent.hpp)
* [SpotLight](SpotLight.hpp), performs lighting for [SpotLightComponents](../../components/data/LightComponent.hpp)
* [PointLight](PointLight.hpp), performs lighting for [PointLightComponents](../../components/data/LightComponent.hpp)

* [ShadowMap](ShadowMap.hpp), used by the lighting shaders
* [ShadowCube](ShadowCube.hpp), used by the lighting shaders

* [GodRaysDirLight](GodRaysDirLight.hpp), performs volumetric lighting for [DirLightComponents](../../components/data/LightComponent.hpp)
* [GodRaysSpotLight](GodRaysSpotLight.hpp), performs volumetric lighting for [SpotLightComponents](../../components/data/LightComponent.hpp)
* [GodRaysPointLight](GodRaysPointLight.hpp), performs volumetric lighting for [PointLightComponents](../../components/data/LightComponent.hpp)

* [LightSphere](LightSphere.hpp), draws spheres where lights are
* [Highlight](Highlight.hpp), highlights `Entities` with a [HighlightComponent](../../components/data/HighlightComponent.md)

The shadow map resolution defaults to 8192 (this seems like a lot, but I've only tested this on a single machine so far) and can be adjusted by defining the `KENGINE_SHADOW_MAP_SIZE` macro.

## Function components

The following `function Components` are implemented:

* [OnMouseCaptured](../../components/data/functions/OnMouseCaptured.md): captures the mouse into the GLFW window
* [GetImGuiScale](../../components/data/functions/GetImGuiScale.md): returns the user-specified ImGui scale
* [GetEntityInPixel](../../components/data/functions/GetEntityInPixel.md)

## Functionality

### Rendering

For each [CameraComponent](../../components/data/CameraComponent.md), all [ShaderComponents](../../components/data/ShaderComponent.md) are processed in the following order:
* GBufferShaderComponents
* LightingShaderComponents
* PostLightingShaderComponents
* PostProcessShaderComponents

### GBuffer management

Upon game initialization, a system should call [kengine::initGBuffer](../../components/functions/InitGBuffer.md) with a type defining the different textures comprising the GBuffer. Here is an example type:

```cpp
struct GBufferTextures {
	float position[4];
	float normal[4];
	float color[4];
	float entityID[4];

	putils_reflection_attributes(
		putils_reflection_attribute(&GBufferTextures::position),
		putils_reflection_attribute(&GBufferTextures::normal),
		putils_reflection_attribute(&GBufferTextures::color),
		putils_reflection_attribute(&GBufferTextures::entityID)
	);
};
```

These GBuffer textures will then be registered with all [ShaderComponents](../../components/data/ShaderComponent.md).

The default shaders provided with this system assume you make use of a type with the same properties as that described above (defined in [kengine::GBufferTextures](OpenGLSystem.hpp)). If you wish to use additional textures in your GBuffer, you can either copy this structure or inherit from it, so long as you are careful to declare the reflectible attributes in the same order (as that order will define their GLSL locations).

### Model construction

[ModelDataComponents](../../components/data/ModelDataComponent.md) are processed to generate meshes and transformed into [SystemSpecificModelComponent](../../components/data/SystemSpecificModelComponent.md).

### Shader initialization and vertex type registration

The shader [Programs](../../putils/opengl/Program.md) for the various [ShaderComponents](../../components/data/ShaderComponent.md) are initialized by the `OpenGLSystem`, and the vertex type registration functions provided by the [ModelDataComponents](../../components/data/ModelDataComponent.md) are called.

### ImGui

The `OpenGLSystem` initializes an ImGui context which can be retrieved through its [ImGuiContextComponent](../../components/data/ImGuiContextComponent.hpp).

If building in debug mode, the following debug elements are automatically added (from [Controllers.hpp](Controllers.hpp)):
* A shader controller, letting you enable/disable individual shaders
* A texture debugger, letting you draw the individual components of the GBuffer or any texture registered by shaders

### Input

Input is captured and transferred to [InputBufferComponents](../../components/data/InputBufferComponent.md).
