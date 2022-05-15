add_library(CruDemoBase INTERFACE)

target_link_libraries(CruDemoBase INTERFACE CruPlatformBootstrap)

if(WIN32)
	add_subdirectory(graphics)

	add_subdirectory(main)
	add_subdirectory(ScrollView)

	add_subdirectory(InputMethod)
	add_subdirectory(SvgPath)
elseif(APPLE)
	add_subdirectory(graphics)

	add_subdirectory(main)
	add_subdirectory(ScrollView)

	add_subdirectory(InputMethod)
	add_subdirectory(SvgPath)
elseif(UNIX)
	add_subdirectory(xcb)
endif()

add_subdirectory(parse)

