add_subdirectory(common)

add_subdirectory(platform)

if(WIN32)
	add_subdirectory(win)
endif()

add_subdirectory(ui)

add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE cru_ui)

if(WIN32)
	target_link_libraries(demo PRIVATE cru_win_native)
endif()
