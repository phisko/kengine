# [OpenGLSpritesSystem](OpenGLSpritesSystem.hpp)

`System` that can render 2D and 3D sprites for the [OpenGLSystem](../opengl/OpenGLSystem.md).

This system creates a [SpritesShader](SpritesShader.hpp) and will perform the necessary texture loading whenever an `Entity` with a [SpriteComponent](../../components/data/SpriteComponent.md) is created.