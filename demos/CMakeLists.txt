add_library(cru_demo_base INTERFACE)

target_link_libraries(cru_demo_base INTERFACE cru_platform_bootstrap)

if(WIN32)
	add_subdirectory(main)
	add_subdirectory(scroll_view)

	add_subdirectory(input_method)
elseif(APPLE)
	add_subdirectory(main)
	add_subdirectory(scroll_view)

	add_subdirectory(input_method)
elseif(UNIX)
	add_subdirectory(xcb)
endif()

if (CRU_BUILD_GRAPHICS_EXPERIMENTS)
	add_subdirectory(graphics_experiments)
endif()
