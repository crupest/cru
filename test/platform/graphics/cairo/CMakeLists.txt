add_executable(CruPlatformGraphicsCairoTest
	BaseTest.cpp
)
target_link_libraries(CruPlatformGraphicsCairoTest PRIVATE CruPlatformGraphicsCairo CruTestBase)

cru_catch_discover_tests(CruPlatformGraphicsCairoTest)
