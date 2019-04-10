add_library(cru_win_base STATIC
	debug.cpp
	exception.cpp
	string_util.cpp
)
target_compile_definitions(cru_win_base PUBLIC UNICODE _UNICODE) # use unicode
target_include_directories(cru_win_base PUBLIC ${PROJECT_SOURCE_DIR}/include .)

add_subdirectory(graph)
add_subdirectory(native)
