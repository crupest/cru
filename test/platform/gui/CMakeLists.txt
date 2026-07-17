add_executable(CruPlatformGuiMockTest
	MockBaseTest.cpp
	MockInputInjectionTest.cpp
	MockServiceTest.cpp
	MockUiApplicationTest.cpp
	MockUserTest.cpp
	MockWindowTest.cpp
)
target_link_libraries(CruPlatformGuiMockTest PRIVATE CruPlatformGuiMock CruTestBase)

cru_catch_discover_tests(CruPlatformGuiMockTest)
