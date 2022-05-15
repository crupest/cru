add_executable(CruPlatformGraphicsDirect2d_test
	ConvertTest.cpp
)
target_link_libraries(CruPlatformGraphicsDirect2d_test PRIVATE CruPlatformGraphicsDirect2d cru_test_base)

if (WIN32)
	add_custom_command(TARGET CruPlatformGraphicsDirect2d_test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CruPlatformGraphicsDirect2d_test> $<TARGET_FILE_DIR:CruPlatformGraphicsDirect2d_test>
	COMMAND_EXPAND_LISTS
	)
endif()

catch_discover_tests(CruPlatformGraphicsDirect2d_test)
