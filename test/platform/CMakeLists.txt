add_executable(CruPlatformBaseTest
	ColorTest.cpp
	DeleteLaterTest.cpp
	GraphicsBaseTest.cpp
	MatrixTest.cpp
)
target_link_libraries(CruPlatformBaseTest PRIVATE CruPlatformBase CruTestBase)

if (WIN32)
	add_subdirectory(graphics/direct2d)
endif()

if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
	add_subdirectory(graphics/cairo)
endif()

cru_catch_discover_tests(CruPlatformBaseTest)
