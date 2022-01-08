add_executable(cru_toml_test
	ParserTest.cpp
)
target_link_libraries(cru_toml_test PRIVATE cru_toml cru_test_base)

gtest_discover_tests(cru_toml_test)
