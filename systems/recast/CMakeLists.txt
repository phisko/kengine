set(name kengine_recast)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine)

putils_conan_download_and_link_packages(
	${name} PRIVATE
	recastnavigation/cci.20200511
	glm/0.9.9.8
)