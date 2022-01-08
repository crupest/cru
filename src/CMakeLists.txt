add_subdirectory(common)

add_subdirectory(platform)

if(WIN32)
	add_subdirectory(win)
elseif (APPLE)
	add_subdirectory(osx)
endif()

add_subdirectory(ui)
add_subdirectory(parse)
add_subdirectory(toml)
add_subdirectory(xml)
