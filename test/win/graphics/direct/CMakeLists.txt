add_executable(cru_win_graphics_direct_test
	ConvertTest.cpp
)
target_link_libraries(cru_win_graphics_direct_test PRIVATE cru_win_graphics_direct cru_test_base)

if (WIN32)
	add_custom_command(TARGET cru_win_graphics_direct_test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:cru_win_graphics_direct_test> $<TARGET_FILE_DIR:cru_win_graphics_direct_test>
	COMMAND_EXPAND_LISTS
	)
endif()

catch_discover_tests(cru_win_graphics_direct_test)
