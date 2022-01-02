add_executable(cru_xml_test
	ParserTest.cpp
)
target_link_libraries(cru_xml_test PRIVATE cru_xml cru_test_base)

gtest_discover_tests(cru_xml_test)
