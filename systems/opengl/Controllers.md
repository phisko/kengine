# [Controllers](Controllers.hpp)

[EntityCreators](../../EntityCreator.md) that create tools used by the [OpenGLSystem](OpenGLSystem.md).

## ShaderController

```cpp
EntityCreator * ShaderController() noexcept;
```

Tool that displays an ImGui window letting users enable or disable specific shaders.

## GBufferDebugger

```cpp
EntityCreatorFunctor<64> GBufferDebugger(const functions::GBufferAttributeIterator & iterator) noexcept;
```

Tool that displays an ImGui window letting users see the various gbuffer attributes of viewports.