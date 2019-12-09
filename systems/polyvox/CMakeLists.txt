set(name kengine_polyvox)

file(GLOB src *.cpp *.hpp)

add_library(${name} STATIC ${src})
target_link_libraries(${name} PUBLIC kengine)
target_include_directories(${name} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/libs/polyvox/include
)
