add_library(CruDemoBase INTERFACE)

target_link_libraries(CruDemoBase INTERFACE CruPlatformBootstrap)

if(WIN32)
	add_subdirectory(Graphics/DrawCircle)

	add_subdirectory(main)
	add_subdirectory(ScrollView)

	add_subdirectory(InputMethod)
	add_subdirectory(SvgPath)
elseif(APPLE)
	add_subdirectory(Graphics/DrawCircle)

	add_subdirectory(main)
	add_subdirectory(ScrollView)

	add_subdirectory(InputMethod)
	add_subdirectory(SvgPath)
elseif(EMSCRIPTEN)

elseif(UNIX)
	add_subdirectory(Graphics/DrawCircle)
	add_subdirectory(xcb)
endif()

add_subdirectory(parse)

