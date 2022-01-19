add_executable(cru_platform_base_test
	ColorTest.cpp
	MatrixTest.cpp
)
target_link_libraries(cru_platform_base_test PRIVATE cru_platform_base cru_test_base)

if (WIN32)
	add_custom_command(TARGET cru_platform_base_test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:cru_platform_base_test> $<TARGET_FILE_DIR:cru_platform_base_test>
	COMMAND_EXPAND_LISTS
	)
endif()

gtest_discover_tests(cru_platform_base_test)
