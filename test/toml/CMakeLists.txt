add_executable(cru_toml_test
	ParserTest.cpp
)
target_link_libraries(cru_toml_test PRIVATE cru_toml cru_test_base)

if (WIN32)
	add_custom_command(TARGET cru_toml_test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:cru_toml_test> $<TARGET_FILE_DIR:cru_toml_test>
	COMMAND_EXPAND_LISTS
	)
endif()

gtest_discover_tests(cru_toml_test)
