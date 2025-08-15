# OPTION A: Build an executable and a static library
	# Executable
	add_executable(executable
		main.cpp)

	target_compile_options(executable PUBLIC -Wall -pedantic)

	target_link_libraries(executable
		PUBLIC
			coverage_config
			module_library)

	target_include_directories(executable
		PRIVATE
			${GENERATED_DIR} )

	install(TARGETS executable DESTINATION ${BIN_DIR})

	# Static library
	file(GLOB FILES "*.cpp")
	list(REMOVE_ITEM FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
	add_library(module_library
			${FILES})

	set_target_properties(module_library PROPERTIES PREFIX "")

	target_compile_options(module_library PUBLIC -Wall -pedantic)

	target_link_libraries(module_library
		PUBLIC
			coverage_config
			libphysica)

	target_include_directories(module_library
		PRIVATE
			${GENERATED_DIR}
		PUBLIC
			${CMAKE_CURRENT_SOURCE_DIR}
			${INCLUDE_DIR})

	install(TARGETS module_library DESTINATION ${LIB_DIR})

#####################################################################
# # OPTION B: Build only an executable.
# Find all source files including main.cpp
# file(GLOB FILES "*.cpp")
# 	# Executable
# 	add_executable(executable
# 		${FILES})

# 	target_compile_options(executable PUBLIC -Wall -pedantic)

# 	target_link_libraries(executable
# 		PUBLIC
# 			coverage_config
# 			libphysica)

# 	target_include_directories(executable
# 		PRIVATE
# 			${GENERATED_DIR} )

# 	install(TARGETS executable DESTINATION ${BIN_DIR})
