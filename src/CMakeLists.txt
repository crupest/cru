add_subdirectory(common)

add_subdirectory(platform)

if (APPLE)
	add_subdirectory(osx)
endif()

add_subdirectory(ui)
add_subdirectory(parse)
add_subdirectory(toml)
add_subdirectory(xml)

add_subdirectory(theme_builder)
