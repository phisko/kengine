# [OpenGLSystem](OpenGLSystem.hpp)

System that renders `Entities` in an OpenGL render window.

The implementation (as well as the shaders) is still a WIP, and I am by no means an expert in computer graphics so this is not expected to be production ready. Heavy-load has not been tested, and bugs are to be expected. If you need to do heavy lifting with 3D graphics, it may be a good idea to implement a new `System` using a higher level library like Ogre3D.

### Shaders

Unless the `KENGINE_OPENGL_NO_DEFAULT_SHADERS` macro is defined, the following shaders are automatically created:

* [DirLight](DirLight.hpp), performs lighting for [DirLightComponents](../../components/LightComponent.hpp)
* [SpotLight](SpotLight.hpp), performs lighting for [SpotLightComponents](../../components/LightComponent.hpp)
* [PointLight](PointLight.hpp), performs lighting for [PointLightComponents](../../components/LightComponent.hpp)

* [ShadowMap](ShadowMap.hpp), used by the lighting shaders
* [ShadowCube](ShadowCube.hpp), used by the lighting shaders

* [GodRaysDirLight](GodRaysDirLight.hpp), performs volumetric lighting for [DirLightComponents](../../components/LightComponent.hpp)
* [GodRaysSpotLight](GodRaysSpotLight.hpp), performs volumetric lighting for [SpotLightComponents](../../components/LightComponent.hpp)
* [GodRaysPointLight](GodRaysPointLight.hpp), performs volumetric lighting for [PointLightComponents](../../components/LightComponent.hpp)

* [LightSphere](LightSphere.hpp), draws spheres where lights are
* [Highlight](Highlight.hpp), highlights `Entities` with a [HighlightComponent](../../components/HighlightComponent.md)

The shadow map resolution defaults to 8192 (this seems like a lot, but I've only tested this on a single machine so far) and can be adjusted by defining the `KENGINE_SHADOW_MAP_SIZE` macro.

### Data packets

The `Entity` seen in a pixel can be queried using the [GetEntityInPixel](../../packets/EntityInPixel.hpp) datapacket.

The size of the GBuffer textures can be queried by using the `GetGBufferSize`(../../packets/GBuffer.hpp) datapacket.

The content of a GBuffer texture can be queried by using the [GetGBufferTexture](../../packets/GBuffer.hpp) datapacket. Take note that each pixel in a texture is formatted as 4 `floats`.
WARNING: textures are flipped vertically (you'll want to do `y = (height - y)` before indexing into them).

Sending the [AddImGuiTool](../../packets/AddImGuiTool.hpp) datapacket will add an entry into the "Tools" section of the ImGui main menu bar, letting users define custom tools windows.

### Functionality

##### Rendering

For each [CameraComponent](../../components/CameraComponent.md), all [ShaderComponents](../../components/ShaderComponent.md) are processed in the following order:
* GBufferShaderComponents
* LightingShaderComponents
* PostProcessShaderComponents

#### GBuffer management

Upon game initialization, a system should call [kengine::initGBuffer](../../packets/GBuffer.hpp) with a type defining the different textures comprising the GBuffer. Here is an example type:

```cpp
struct GBufferTextures {
	float position[4];
	float normal[4];
	float color[4];
	float entityID[4];

	pmeta_get_attributes(
		pmeta_reflectible_attribute(&GBufferTextures::position),
		pmeta_reflectible_attribute(&GBufferTextures::normal),
		pmeta_reflectible_attribute(&GBufferTextures::color),
		pmeta_reflectible_attribute(&GBufferTextures::entityID)
	);
};
```

These GBuffer textures will then be registered with all [ShaderComponents](../../components/ShaderComponent.md).

The default shaders provided with this system assume you make use of a type with the same properties as that described above (defined in [kengine::GBufferTextures](OpenGLSystem.hpp)). If you wish to use additional textures in your GBuffer, you can either copy this structure or inherit from it, so long as you are careful to declare the reflectible attributes in the same order (as that order will define their GLSL locations).

##### Model construction

[ModelLoaderComponents](../../components/ModelLoaderComponent.md) are processed to generate meshes and transformed into [ModelInfoComponents](../../components/ModelInfoComponent.md).

##### Shader initialization and vertex type registration

The shader [Programs](../../../putils/opengl/Program.md) for the various [ShaderComponents](../../components/ShaderComponent.md) are initialized by the `OpenGLSystem`, and the vertex type registration functions provided by the [ModelLoaderComponents](../../components/ModelLoaderComponent.md) are called.

##### ImGui

ImGui elements can be rendered by [ImGuiComponents](../../components/ImGuiComponent.md).

If building in debug mode, the following debug elements are automatically added (from [Controllers.hpp](Controllers.hpp)):
* A shader controller, letting you enable/disable individual shaders
* A light debugger, letting you adjust the properties of [LightComponents](../../components/LightComponent.md)
* A texture debugger, letting you draw the individual components of the GBuffer or any texture registered by shaders

##### Input

Input is captured and transferred to [InputComponents](../../components/InputComponent.md).