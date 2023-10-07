add_executable(CruXmlTest
	ParserTest.cpp
)
target_link_libraries(CruXmlTest PRIVATE CruXml CruTestBase)

cru_catch_discover_tests(CruXmlTest)
