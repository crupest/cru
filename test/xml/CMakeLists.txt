add_executable(CruXmlTest
	ParserTest.cpp
)
target_link_libraries(CruXmlTest PRIVATE CruXml cru_test_base)

if (WIN32)
	add_custom_command(TARGET CruXmlTest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CruXmlTest> $<TARGET_FILE_DIR:CruXmlTest>
	COMMAND_EXPAND_LISTS
	)
endif()

catch_discover_tests(CruXmlTest)
