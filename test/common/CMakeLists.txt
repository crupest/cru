add_executable(CruBaseTest
	Event2Test.cpp
	HandlerRegistryTest.cpp
	PropertyTreeTest.cpp
	SelfResolvableTest.cpp
	StringTest.cpp
	StringToNumberConverterTest.cpp
	StringUtilTest.cpp
)
target_link_libraries(CruBaseTest PRIVATE CruBase CruTestBase)

if (UNIX AND NOT EMSCRIPTEN)
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

cru_catch_discover_tests(CruBaseTest)
