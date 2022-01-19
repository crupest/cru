add_executable(cru_xml_test
	ParserTest.cpp
)
target_link_libraries(cru_xml_test PRIVATE cru_xml cru_test_base)

add_custom_command(TARGET cru_xml_test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:cru_xml_test> $<TARGET_FILE_DIR:cru_xml_test>
  COMMAND_EXPAND_LISTS
  )

gtest_discover_tests(cru_xml_test)
