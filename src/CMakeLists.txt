# OPTION A: Build an executable and a static library
# Executable
add_executable(raytracer
	main.cpp)

target_compile_options(raytracer PUBLIC -Wall -pedantic)

target_link_libraries(raytracer
	PUBLIC
	coverage_config
	libraytracer)

target_include_directories(raytracer
	PRIVATE
	${GENERATED_DIR})

install(TARGETS raytracer DESTINATION ${BIN_DIR})

# Static library
file(GLOB FILES "*.cpp")
list(REMOVE_ITEM FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_library(libraytracer
	${FILES})

set_target_properties(libraytracer PROPERTIES PREFIX "")

target_compile_options(libraytracer PUBLIC -Wall -pedantic)

target_link_libraries(libraytracer
	PUBLIC
	coverage_config
	libphysica
	yaml-cpp)

target_include_directories(libraytracer
	PRIVATE
	${GENERATED_DIR}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${INCLUDE_DIR}
	${STB_DIR}
	${YAMLCPPDIR})

install(TARGETS libraytracer DESTINATION ${LIB_DIR})