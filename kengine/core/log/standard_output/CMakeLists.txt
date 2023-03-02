project(kengine)

find_path(TERMCOLOR_INCLUDE_DIRS "termcolor/termcolor.hpp")
target_include_directories(${kengine_library_name} PRIVATE ${TERMCOLOR_INCLUDE_DIRS})