# [kreogl](kreogl.hpp)

System that uses the [kreogl](https://github.com/phisko/kreogl) library to render entities in 3D.

A custom [highlight_shader](../shaders/highlight_shader.hpp) is implemented, which highlights entities` with a [highlight component](../../data/highlight.md).

Adding user-defined shaders is not implemented in this first draft, but may be done easily in the future by adding some sort of `kreogl_shader` component.