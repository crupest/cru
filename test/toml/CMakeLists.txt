add_executable(CruTomlTest
	ParserTest.cpp
)
target_link_libraries(CruTomlTest PRIVATE CruToml CruTestBase)

cru_catch_discover_tests(CruTomlTest)
