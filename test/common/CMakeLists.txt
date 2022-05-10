add_executable(cru_base_test
	HandlerRegistryTest.cpp
	PropertyTreeTest.cpp
	StringTest.cpp
	StringToNumberConverterTest.cpp
	StringUtilTest.cpp
)
target_link_libraries(cru_base_test PRIVATE cru_base cru_test_base)

if (UNIX)
	target_sources(cru_base_test PRIVATE
		platform/unix/UnixFileStreamTest.cpp
	)
endif()

if (WIN32)
	target_sources(cru_base_test PRIVATE
		platform/win/StreamConvertTest.cpp
		platform/win/Win32FileStreamTest.cpp
	)

	add_custom_command(TARGET cru_base_test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:cru_base_test> $<TARGET_FILE_DIR:cru_base_test>
	COMMAND_EXPAND_LISTS
	)
endif()

catch_discover_tests(cru_base_test)
