# Allows to add test cases using Google Test
#

# recommended to use
#
# export GTEST_CATCH_EXCEPTIONS=0
#
# which disables a feature that seems to be popup-blocker for Windows


macro (add_gtest source)
	if (BUILD_TESTING)
	set (SOURCES ${HDR_FILES} ${source}.cpp)
	add_executable (${source} ${SOURCES})

	#do not hardcode tools here:
	target_link_elektratools(${source})

	target_link_libraries(${source} gtest gtest_main)
	include_directories(SYSTEM ${GOOGLETEST_ROOT}/include)
	include_directories(${CMAKE_SOURCE_DIR}/tests/gtest-framework)

	if (INSTALL_TESTING)
		install (TARGETS ${source}
			DESTINATION ${TARGET_TOOL_EXEC_FOLDER})
	endif (INSTALL_TESTING)

	set_target_properties (${source} PROPERTIES
			COMPILE_DEFINITIONS HAVE_KDBCONFIG_H)

	add_test (${source}
			"${CMAKE_BINARY_DIR}/bin/${source}"
			"${CMAKE_CURRENT_BINARY_DIR}/"
			)
	endif(BUILD_TESTING)
endmacro (add_gtest)
