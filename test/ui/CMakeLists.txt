add_executable(CruUiMockTest
	MockControlHostTest.cpp
	MockPlatformSeamTest.cpp
)
target_link_libraries(CruUiMockTest PRIVATE CruUi CruPlatformGuiMock CruTestBase)

cru_catch_discover_tests(CruUiMockTest)
