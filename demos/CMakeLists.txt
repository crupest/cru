add_library(CruDemoBase INTERFACE)

target_link_libraries(CruDemoBase INTERFACE CruPlatformBootstrap)

add_subdirectory(Graphics)

if(WIN32)
	add_subdirectory(main)
	add_subdirectory(ScrollView)
	add_subdirectory(InputMethod)
elseif(APPLE)
	add_subdirectory(main)
	add_subdirectory(ScrollView)
	add_subdirectory(InputMethod)
elseif(UNIX)
	add_subdirectory(xcb)
endif()

add_subdirectory(parse)

