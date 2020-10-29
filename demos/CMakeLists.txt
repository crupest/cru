add_library(cru_demo_base INTERFACE)

if(WIN32)
	target_link_libraries(cru_demo_base INTERFACE cru_win_gui)
endif()

add_subdirectory(main)
add_subdirectory(input_method)
