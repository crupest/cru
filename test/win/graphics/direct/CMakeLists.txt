add_executable(CruPlatformGraphicsDirect2dTest
	ConvertTest.cpp
)
target_link_libraries(CruPlatformGraphicsDirect2dTest PRIVATE CruPlatformGraphicsDirect2d cru_test_base)

if (WIN32)
	add_custom_command(TARGET CruPlatformGraphicsDirect2dTest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CruPlatformGraphicsDirect2dTest> $<TARGET_FILE_DIR:CruPlatformGraphicsDirect2dTest>
	COMMAND_EXPAND_LISTS
	)
endif()

catch_discover_tests(CruPlatformGraphicsDirect2dTest)
