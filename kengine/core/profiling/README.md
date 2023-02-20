# kengine_core_profiling

Profiling macros wrapping [the putils profiling system](https://github.com/phisko/putils/blob/main/putils/profiling.md). To enable profiling, set the `KENGINE_PROFILING` CMake variable to `ON`.

## Cross-DLL usage

If multiple DLLs are going to link against `kengine` with `KENGINE_PROFILING` enabled, you'll want to set the `TRACY_STATIC` CMake variable to `OFF`, to make sure all DLLs share a single instance of Tracy. You'll also want to use the [putils_copy_dlls](https://github.com/phisko/cmake_helpers/blob/main/CMakeModules/putils_copy_dlls.cmake) CMake function to make sure the Tracy DLL is copied next to your executable.

* [helpers](helpers)
	* [kengine_profiling_frame](helpers/kengine_profiling_frame.md): start a new profiling frame
	* [kengine_profiling_scope](helpers/kengine_profiling_scope.md): instrument a scope