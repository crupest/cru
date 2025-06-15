include(Catch)

function(cru_catch_discover_tests target)
	if (EMSCRIPTEN)
		message("Emscripten, not do catch_discover_tests for " ${target})
	else()
		catch_discover_tests(${target})
	endif()
endfunction()

add_library(CruTestBase INTERFACE)
if (EMSCRIPTEN)
	target_link_libraries(CruTestBase INTERFACE Catch2::Catch2)
else()
	target_link_libraries(CruTestBase INTERFACE Catch2::Catch2WithMain)
endif()

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

add_executable(CruTestHelperEcho SubProcessHelper/CruEcho.cpp)
add_executable(CruTestHelperTee SubProcessHelper/CruTee.cpp)
target_compile_definitions(CruBaseTest PRIVATE
	CRU_TEST_HELPER_ECHO_LOCATION="$<TARGET_FILE:CruTestHelperEcho>"
	CRU_TEST_HELPER_TEE_LOCATION="$<TARGET_FILE:CruTestHelperTee>"
)

if (UNIX AND NOT EMSCRIPTEN)
	target_sources(CruBaseTest PRIVATE UnixTest.cpp)
endif()

cru_catch_discover_tests(CruBaseTest)

add_subdirectory(platform)
add_subdirectory(xml)
add_subdirectory(toml)
add_subdirectory(ui)
