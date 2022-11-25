set(name kengine_polyvox)

file(GLOB src *.cpp *.hpp)

add_library(${name} ${src})
target_link_libraries(${name} PRIVATE kengine_core)
target_include_directories(${name} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/libs/polyvox/include
)
putils_export_symbols(${name})