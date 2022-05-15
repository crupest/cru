add_executable(CruPlatformBaseTest
	ColorTest.cpp
	MatrixTest.cpp
)
target_link_libraries(CruPlatformBaseTest PRIVATE CruPlatformBase cru_test_base)

if (WIN32)
	add_custom_command(TARGET CruPlatformBaseTest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CruPlatformBaseTest> $<TARGET_FILE_DIR:CruPlatformBaseTest>
	COMMAND_EXPAND_LISTS
	)
endif()

catch_discover_tests(CruPlatformBaseTest)
