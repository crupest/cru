add_library(CruDemoBase INTERFACE)

target_link_libraries(CruDemoBase INTERFACE CruPlatformBootstrap)

add_subdirectory(platform)
add_subdirectory(main)
add_subdirectory(ScrollView)
add_subdirectory(InputMethod)

if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
	add_subdirectory(xcb)
endif()

add_subdirectory(parse)

