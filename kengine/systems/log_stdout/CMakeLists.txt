set(name kengine_log_stdout)

file(GLOB src *.cpp *.hpp)
add_library(${name} ${src})
target_link_libraries(${name} PRIVATE kengine_core)
putils_export_symbols(${name})

putils_conan_download_and_link_packages(${name} PRIVATE termcolor/2.0.0)