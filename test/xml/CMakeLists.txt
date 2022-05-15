add_executable(CruXmlTest
	ParserTest.cpp
)
target_link_libraries(CruXmlTest PRIVATE CruXml CruTestBase)

catch_discover_tests(CruXmlTest)
