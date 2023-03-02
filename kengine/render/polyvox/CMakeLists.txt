project(kengine)

target_include_directories(${kengine_library_name} PUBLIC libs/polyvox/include)
subdirectory_is_not_kengine_library(libs)