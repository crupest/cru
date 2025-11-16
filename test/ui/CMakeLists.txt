add_executable(CruUiTest
	DeleteLaterTest.cpp
)
target_link_libraries(CruUiTest PRIVATE CruUi CruTestBase)

cru_catch_discover_tests(CruUiTest)
