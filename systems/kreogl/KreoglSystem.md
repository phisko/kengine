# [KreoglSystem](KreoglSystem.hpp)

System that uses the [kreogl](https://github.com/phisko/kreogl) library to render `Entities` in 3D.

A custom [HighlightShader](shaders/HighlightShader.hpp) is implemented, which highlights `Entities` with a [HighlightComponent](../../components/data/HighlightComponent.md).

Adding user-defined shaders is not implemented in this first draft, but may be done easily in the future by adding some sort of `KreoglShaderComponent`.