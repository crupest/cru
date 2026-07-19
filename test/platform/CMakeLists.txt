add_executable(CruPlatformBaseTest
	ColorTest.cpp
	DeleteLaterTest.cpp
	GraphicsBaseTest.cpp
	MatrixTest.cpp
)
target_link_libraries(CruPlatformBaseTest PRIVATE CruPlatformBase CruTestBase)

add_subdirectory(graphics)

if (TARGET CruPlatformGraphicsDirect2d)
	add_subdirectory(graphics/direct2d)
endif()

if (TARGET CruPlatformGraphicsCairo)
	add_subdirectory(graphics/cairo)
endif()

cru_catch_discover_tests(CruPlatformBaseTest)
