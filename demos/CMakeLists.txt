add_library(cru_demo_base INTERFACE)

if(WIN32)
	target_link_libraries(cru_demo_base INTERFACE cru_win_gui)
endif()

if(WIN32) # Currently only enable tests on Windows.
	add_subdirectory(main)
	add_subdirectory(scroll_view)

	add_subdirectory(input_method)
endif()

if(UNIX)
	add_subdirectory(xcb)
endif()
