add_library(cru_demo_base INTERFACE)

target_link_libraries(cru_demo_base INTERFACE cru_platform_boostrap)

if(WIN32) # Currently only enable tests on Windows.
	add_subdirectory(main)
	add_subdirectory(scroll_view)

	add_subdirectory(input_method)
endif()

if(UNIX AND NOT APPLE)
	add_subdirectory(xcb)
endif()
