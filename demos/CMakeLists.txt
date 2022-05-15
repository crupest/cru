add_library(cru_demo_base INTERFACE)

target_link_libraries(cru_demo_base INTERFACE CruPlatformBootstrap)

if(WIN32)
	add_subdirectory(graphics)

	add_subdirectory(main)
	add_subdirectory(scroll_view)

	add_subdirectory(input_method)
	add_subdirectory(svg_path)
elseif(APPLE)
	add_subdirectory(graphics)

	add_subdirectory(main)
	add_subdirectory(scroll_view)

	add_subdirectory(input_method)
	add_subdirectory(svg_path)
elseif(UNIX)
	add_subdirectory(xcb)
endif()

add_subdirectory(parse)

