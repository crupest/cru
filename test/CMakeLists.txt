include(Catch)

add_library(cru_test_base INTERFACE)
target_link_libraries(cru_test_base INTERFACE Catch2::Catch2WithMain)

add_subdirectory(common)
add_subdirectory(platform)
add_subdirectory(xml)
add_subdirectory(toml)
add_subdirectory(ui)

if(WIN32)
	add_subdirectory(win)
endif()
