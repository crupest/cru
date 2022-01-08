add_executable(cru_base_test
	HandlerRegistryTest.cpp
	PropertyTreeTest.cpp
	StringTest.cpp
	StringUtilTest.cpp
)
target_link_libraries(cru_base_test PRIVATE cru_base cru_test_base)

gtest_discover_tests(cru_base_test)
