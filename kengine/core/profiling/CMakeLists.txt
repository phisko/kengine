project(kengine)

option(KENGINE_PROFILING "Enable profiling")
if(KENGINE_PROFILING)
    set(PUTILS_PROFILING ON CACHE BOOL "Enabled for kengine_core_profiling" FORCE)
endif()