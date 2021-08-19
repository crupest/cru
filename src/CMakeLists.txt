add_subdirectory(common)

add_subdirectory(platform)

if(WIN32)
	add_subdirectory(win)
elseif (APPLE)
	add_subdirectory(osx)
endif()

add_subdirectory(ui)

