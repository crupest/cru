add_executable(cru_base_test
	HandlerRegistryTest.cpp
	PropertyTreeTest.cpp
	StringTest.cpp
	StringUtilTest.cpp
)
target_link_libraries(cru_base_test PRIVATE cru_base cru_test_base)

if (UNIX)
	target_sources(cru_base_test PRIVATE
		io/UnixFileStreamTest.cpp
	)
endif()

if (WIN32)
	add_custom_command(TARGET cru_base_test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:cru_base_test> $<TARGET_FILE_DIR:cru_base_test>
	COMMAND_EXPAND_LISTS
	)
endif()

gtest_discover_tests(cru_base_test)
