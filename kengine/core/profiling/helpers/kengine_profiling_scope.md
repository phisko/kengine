# [profiling_helper](profiling_helper.hpp)

Profiling macros, wrapping [the putils profiling system](https://github.com/phisko/putils/blob/main/putils/profiling.md). To enable profiling, set the `KENGINE_PROFILING` CMake variable to `ON`.

## Members

### KENGINE_PROFILING_SCOPE

Placed at the start of a function, instruments it.

### KENGINE_PROFILING_FRAME

Placed in client code to mark the end of a frame. The [main_loop](../../main_loop/helpers/main_loop.md) helper takes care of this, if you're using it.

## Cross-DLL usage

If multiple DLLs are going to link against `kengine` with `KENGINE_PROFILING` enabled, you'll want to set the `TRACY_STATIC` CMake variable to `OFF`, to make sure all DLLs share a single instance of Tracy. You'll also want to use the [putils_copy_dlls](https://github.com/phisko/cmake_helpers/blob/main/CMakeModules/putils_copy_dlls.cmake) CMake function to make sure the Tracy DLL is copied next to your executable.