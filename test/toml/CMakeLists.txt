add_executable(CruTomlTest
	ParserTest.cpp
)
target_link_libraries(CruTomlTest PRIVATE CruToml cru_test_base)

if (WIN32)
	add_custom_command(TARGET CruTomlTest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CruTomlTest> $<TARGET_FILE_DIR:CruTomlTest>
	COMMAND_EXPAND_LISTS
	)
endif()

catch_discover_tests(CruTomlTest)
