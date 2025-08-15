# 0. Test executable
add_test(NAME test_Executable COMMAND executable
	WORKING_DIRECTORY ${BIN_DIR})

# 1. Test all modules
file(GLOB TESTFILES "*.cpp")

#####################################################################
# Option A: If a static library is built. (see src/CMakeLists.txt)
foreach(TESTFILE ${TESTFILES})
	get_filename_component(TESTNAME ${TESTFILE} NAME_WLE)
	get_filename_component(FILENAME ${TESTFILE} NAME)
	
	add_executable(${TESTNAME} ${FILENAME})
	
	target_link_libraries( ${TESTNAME} 
	PRIVATE
		module_library
		gtest_main	#contains the main function
	)
	
	target_include_directories(${TESTNAME} PRIVATE ${GENERATED_DIR} )
	
	target_compile_options(${TESTNAME} PUBLIC -Wall -pedantic)
	
	install(TARGETS ${TESTNAME} DESTINATION ${TESTS_DIR})
	
	add_test(NAME ${TESTNAME} COMMAND ${TESTNAME}
		WORKING_DIRECTORY ${TESTS_DIR})
endforeach()

#####################################################################
# Option B: If no static library is built.(see src/CMakeLists.txt)
# foreach(TESTFILE ${TESTFILES})
# 	get_filename_component(TESTNAME ${TESTFILE} NAME_WLE)
# 	get_filename_component(FILENAME ${TESTFILE} NAME)
# 	string(SUBSTRING ${FILENAME} 5 -1 MODULENAME) # Test files must be named test_XXX.cpp where XXX is the name of the module, i.e. XXX.xpp
# 	message("MODULENAME: ${MODULENAME}")
# 	add_executable(${TESTNAME}
# 		test_module.cpp
# 		"${SRC_DIR}/${MODULENAME}")

# 	target_link_libraries(${TESTNAME}
# 		PUBLIC coverage_config 
# 		PRIVATE gtest_main	#contains the main function
#		libphysica
# 	)

# 	target_include_directories(${TESTNAME} PRIVATE ${GENERATED_DIR} )

# 	target_compile_options(${TESTNAME} PUBLIC -Wall -pedantic)
	
# 	install(TARGETS ${TESTNAME} DESTINATION ${TESTS_DIR})

# 	add_test(NAME ${TESTNAME} COMMAND ${TESTNAME}
# 		WORKING_DIRECTORY ${TESTS_DIR})

# endforeach()