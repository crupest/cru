add_executable(CruPlatformGuiMockTest
	MockBaseTest.cpp
	MockInputInjectionTest.cpp
	MockServiceTest.cpp
	MockTesterTest.cpp
	MockUiApplicationTest.cpp
	MockWindowTest.cpp
)
target_link_libraries(CruPlatformGuiMockTest PRIVATE CruPlatformGuiMock CruTestBase)

cru_catch_discover_tests(CruPlatformGuiMockTest)
