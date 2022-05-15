add_executable(CruBaseTest
	HandlerRegistryTest.cpp
	PropertyTreeTest.cpp
	StringTest.cpp
	StringToNumberConverterTest.cpp
	StringUtilTest.cpp
)
target_link_libraries(CruBaseTest PRIVATE CruBase cru_test_base)

if (UNIX)
	target_sources(CruBaseTest PRIVATE
		platform/unix/UnixFileStreamTest.cpp
	)
endif()

if (WIN32)
	target_sources(CruBaseTest PRIVATE
		platform/win/StreamConvertTest.cpp
		platform/win/Win32FileStreamTest.cpp
	)

	add_custom_command(TARGET CruBaseTest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CruBaseTest> $<TARGET_FILE_DIR:CruBaseTest>
	COMMAND_EXPAND_LISTS
	)
endif()

catch_discover_tests(CruBaseTest)
