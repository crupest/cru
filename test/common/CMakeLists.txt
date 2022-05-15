add_executable(CruBaseTest
	HandlerRegistryTest.cpp
	PropertyTreeTest.cpp
	StringTest.cpp
	StringToNumberConverterTest.cpp
	StringUtilTest.cpp
)
target_link_libraries(CruBaseTest PRIVATE CruBase CruTestBase)

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
endif()

catch_discover_tests(CruBaseTest)
