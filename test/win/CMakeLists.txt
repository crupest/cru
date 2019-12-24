add_executable(cru_test_win_base
	string.cpp
)
target_link_libraries(cru_test_win_base PRIVATE cru_win_base cru_test_base)

gtest_discover_tests(cru_test_win_base)
