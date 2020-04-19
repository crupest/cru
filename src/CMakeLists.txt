add_subdirectory(common)

add_subdirectory(platform)

if(WIN32)
	add_subdirectory(win)
endif()

add_subdirectory(ui)

