add_executable(CruBaseTest
	Event2Test.cpp
	HandlerRegistryTest.cpp
	PropertyTreeTest.cpp
	SelfResolvableTest.cpp
	StringTest.cpp
	StringToNumberConverterTest.cpp
	StringUtilTest.cpp
	SubProcessTest.cpp
)
target_link_libraries(CruBaseTest PRIVATE CruBase CruTestBase)

add_executable(CruTestHelperEcho
	SubProcessHelper/CruEcho.cpp
)

add_executable(CruTestHelperTee
	SubProcessHelper/CruTee.cpp
)

target_compile_definitions(CruBaseTest PRIVATE
	CRU_TEST_HELPER_ECHO_LOCATION="$<TARGET_FILE:CruTestHelperEcho>"
	CRU_TEST_HELPER_TEE_LOCATION="$<TARGET_FILE:CruTestHelperTee>"
)

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
